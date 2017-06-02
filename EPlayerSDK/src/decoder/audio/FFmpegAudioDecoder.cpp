/*
 * ****************************************************************
 * This software is a media player SDK implementation
 * GPL:
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details. You should
 * have received a copy of the GNU Library General Public License
 * along with this library; if not, write to the Free Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Project: EC < Enjoyable Coding >
 *
 * FFmpegAudioDecoder.cpp
 * This file for all FFMPEG decoder interface implement.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#include "ECMemOP.h"
#include "AudioDecoder.h"
#include "FFmpegAudioDecoder.h"


FFmpegAudioDecoder::FFmpegAudioDecoder()
:m_pPCM(NULL)
,m_TimeBase(0)
,m_pCodecCtx(NULL)
,m_pCodecParam(NULL)
,m_nMaxPCMBufSize(0)
{
    ECMemSet(&m_AudioFrameBuf, 0, sizeof(AudioFrame));
}

FFmpegAudioDecoder::~FFmpegAudioDecoder()
{
    if (m_AudioFrameBuf.nBufSize && m_AudioFrameBuf.pPCMBuf)
    {
        ECMemFree(m_AudioFrameBuf.pPCMBuf);
    }
}

EC_U32 FFmpegAudioDecoder::OpenDecoder(MediaContext* pMediaContext)
{
    if (NULL == pMediaContext)
        return EC_Err_BadParam;

    m_pCodecCtx = pMediaContext->pAudioCodecCtx;
    m_pCodecParam = pMediaContext->pAudioDecParam;
    AVStream* pStream = pMediaContext->pFormatCtx->streams[pMediaContext->nAudioIndex];

    m_TimeBase = av_q2d(pStream->time_base);

    AVCodec* pCodec = avcodec_find_decoder(m_pCodecCtx->codec_id);
    av_codec_set_pkt_timebase(m_pCodecCtx, m_pCodecCtx->time_base);

    if (NULL == pCodec)
    {
        return Audio_Dec_Err_OpenFaild;
    }
    if (avcodec_open2(m_pCodecCtx, pCodec, NULL) < 0)
    {
        return Audio_Dec_Err_OpenFaild;
    }
    m_pPCM = av_frame_alloc();
    if (NULL == m_pPCM)
    {
        return EC_Err_Memory_Low;
    }

    return Audio_Dec_Err_None;
}

void FFmpegAudioDecoder::CloseDecoder()
{
    if(m_pPCM)
    {
        av_free(m_pPCM);
        m_pPCM = NULL;
    }
    avcodec_close(m_pCodecCtx);
    m_AudioFrameBuf.nSamples = 0;
    m_AudioFrameBuf.nDataSize = 0;
    m_AudioFrameBuf.nTimestamp = 0;
}

void FFmpegAudioDecoder::FlushDecoder()
{
    int ret = 0;
    AVFrame* pFlushFrame = av_frame_alloc();
    AVPacket *pFlushPacket = av_packet_alloc();
    pFlushPacket->size = 0;
    pFlushPacket->data = NULL;

    avcodec_send_packet(m_pCodecCtx, pFlushPacket);
    do
    {
        ret = avcodec_receive_frame(m_pCodecCtx, pFlushFrame);
    } while (ret == 0);
    avcodec_flush_buffers(m_pCodecCtx);

    av_frame_free(&pFlushFrame);
    av_packet_free(&pFlushPacket);

    m_AudioFrameBuf.nSamples = 0;
    m_AudioFrameBuf.nDataSize = 0;
    m_AudioFrameBuf.nTimestamp = 0;
}

EC_U32 FFmpegAudioDecoder::SetInputPacket(SourcePacket* pInputPacket)
{
    int ret = avcodec_send_packet(m_pCodecCtx, pInputPacket);
    if (ret == 0)
        return Audio_Dec_Err_None;
    if (ret == AVERROR(EAGAIN))
        return Audio_Dec_Err_KeepPkt;
    else
        return Audio_Dec_Err_SkipPkt;
}

EC_U32 FFmpegAudioDecoder::GetOutputFrame(AudioFrame* pOutputAudioFrame, bool rawData)
{
    int ret = avcodec_receive_frame(m_pCodecCtx, m_pPCM);
    if (ret == AVERROR(EAGAIN))
        return Audio_Dec_Err_NeedNewPkt;

    if (ret == Audio_Dec_Err_None)
    {
        int i, j;
        int nChannels = m_pCodecParam->channels;
        int nSamples = m_pCodecParam->frame_size;
        int nSampleSize = av_get_bytes_per_sample((AVSampleFormat)m_pCodecParam->format);
        if(nSamples <=0)
        {
            nSamples = m_pPCM->nb_samples;
        }
        int nFrameSize = nChannels * nSamples * nSampleSize;

        if (rawData)
        {
            if (m_AudioFrameBuf.nBufSize && m_AudioFrameBuf.pPCMBuf)
            {
                m_AudioFrameBuf.nBufSize = 0;
                ECMemFree(m_AudioFrameBuf.pPCMBuf);
                m_AudioFrameBuf.pPCMBuf = NULL;
            }
            m_AudioFrameBuf.pPCMBuf = (char*)m_pPCM->data;
            m_AudioFrameBuf.nDataSize = m_AudioFrameBuf.nDataSize + nFrameSize;
        }
        else
        {
            int nFreeSize = m_AudioFrameBuf.nBufSize - m_AudioFrameBuf.nDataSize;
            if (nFreeSize < nFrameSize)
            {
                EC_U32 nNewBufSize = m_AudioFrameBuf.nDataSize + (nFrameSize * 2);
                char* pNewBuf = (char*)ECMemAlloc(nNewBufSize);

                if (m_AudioFrameBuf.nDataSize > 0)
                {
                    ECMemCopy(pNewBuf, m_AudioFrameBuf.pPCMBuf, m_AudioFrameBuf.nDataSize);
                    ECMemFree(m_AudioFrameBuf.pPCMBuf);
                }
                m_AudioFrameBuf.pPCMBuf = pNewBuf;
                m_AudioFrameBuf.nBufSize = nNewBufSize;
                m_AudioFrameBuf.nDataSize = m_AudioFrameBuf.nDataSize;
            }
            /* Copy PCM data */
            for (i = 0; i < nSamples; i++)
            {
                for (j = 0; j < nChannels; j++)
                {
                    uint8_t *dataPtr = m_pPCM->data[j] + nSampleSize * i;
                    uint8_t *buffPtr = (uint8_t*)m_AudioFrameBuf.pPCMBuf + m_AudioFrameBuf.nDataSize;

                    ECMemCopy(buffPtr, dataPtr, nSampleSize);
                    m_AudioFrameBuf.nDataSize = m_AudioFrameBuf.nDataSize + nSampleSize;
                }
            }
        }
        m_AudioFrameBuf.nSamples = nSamples;

        EC_U64 nTimeStamp = 0;
        if (m_pPCM->pts != AV_NOPTS_VALUE)
            nTimeStamp = m_pPCM->pts;
        else if (m_pPCM->pkt_dts != AV_NOPTS_VALUE)
            nTimeStamp = m_pPCM->pkt_dts;
        m_AudioFrameBuf.nTimestamp = m_TimeBase * nTimeStamp * TIME_UNIT;

        pOutputAudioFrame->pPCMData = m_AudioFrameBuf.pPCMBuf;
        pOutputAudioFrame->nDataSize = m_AudioFrameBuf.nDataSize;
        pOutputAudioFrame->nSamples = m_AudioFrameBuf.nSamples;
        pOutputAudioFrame->nTimestamp = m_AudioFrameBuf.nTimestamp;

        m_AudioFrameBuf.nSamples = 0;
        m_AudioFrameBuf.nDataSize = 0;
    }
    return ret;
}
