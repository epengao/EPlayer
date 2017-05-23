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
 * FFmpegVideoDecoder.cpp
 * This file for video FFMPEG decoder interface implement.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#include "ECMemOP.h"
#include "FFmpegVideoDecoder.h"


FFmpegVideoDecoder::FFmpegVideoDecoder()
:m_TimeBase(0)
,m_pFrame(NULL)
,m_pCodecCtx(NULL)
,m_pCodecParam(NULL)
,m_nMaxPCMBufSize(0)
{
}

FFmpegVideoDecoder::~FFmpegVideoDecoder()
{
}

EC_U32 FFmpegVideoDecoder::OpenDecoder(MediaContext* pMediaContext)
{
    if (NULL == pMediaContext)
        return EC_Err_BadParam;

    m_pCodecCtx = pMediaContext->pVideoCodecCtx;
    m_pCodecParam = pMediaContext->pVideoDecParam;
    AVStream* pStream = pMediaContext->pFormatCtx->streams[pMediaContext->nVideoIndex];

    m_TimeBase = av_q2d(pStream->time_base);

    AVCodec* pCodec = avcodec_find_decoder(m_pCodecCtx->codec_id);
    av_codec_set_pkt_timebase(m_pCodecCtx, m_pCodecCtx->time_base);

    if (NULL == pCodec)
    {
        return Video_Dec_Err_OpenFaild;
    }
    if (avcodec_open2(m_pCodecCtx, pCodec, NULL) < 0)
    {
        return Video_Dec_Err_OpenFaild;
    }
    m_pFrame = av_frame_alloc();
    if (NULL == m_pFrame)
    {
        return EC_Err_Memory_Low;
    }

    return Video_Dec_Err_None;
}

void FFmpegVideoDecoder::CloseDecoder()
{
    av_free(m_pFrame);
    avcodec_close(m_pCodecCtx);
    m_pCodecCtx = NULL;
}

void FFmpegVideoDecoder::FlushDecoder()
{
    int ret = 0;
    m_pFrame->pts = 0;
    m_pFrame->pkt_dts = 0;

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
}

EC_U32 FFmpegVideoDecoder::SetInputPacket(SourcePacket* pInputPacket)
{
    int ret = avcodec_send_packet(m_pCodecCtx, pInputPacket);
    if (ret == 0)
        return Video_Dec_Err_None;
    if (ret == AVERROR(EAGAIN))
        return Video_Dec_Err_KeepPkt;
    else
        return Video_Dec_Err_SkipPkt;
}

EC_U32 FFmpegVideoDecoder::GetOutputFrame(VideoFrame* pOutputVideoFrame)
{
    EC_U32 ret = avcodec_receive_frame(m_pCodecCtx, m_pFrame);
    if(ret == 0)
    {
        EC_U64 nTimeStamp = 0;
        if(m_pFrame->pts != AV_NOPTS_VALUE)
        {
            nTimeStamp = m_pFrame->pts;
        }
        else if(m_pFrame->pkt_dts != AV_NOPTS_VALUE)
        {
            nTimeStamp = m_pFrame->pkt_dts;
        }
        pOutputVideoFrame->pAVFrame = m_pFrame;
        pOutputVideoFrame->nTimestamp = m_TimeBase * nTimeStamp * TIME_UNIT;
    }
    else if (ret == AVERROR(EAGAIN))
    {
        ret = Video_Dec_Err_NeedNewPkt;
    }
    return ret;
}
