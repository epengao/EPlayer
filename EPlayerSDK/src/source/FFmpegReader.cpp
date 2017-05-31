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
 * FFmpegReader.cpp
 * This file for all FFMPEG source interface implement.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#include "ECLog.h"
#include "ECUtil.h"
#include "ECError.h"
#include "ECMemOP.h"
#include "MediaSource.h"
#include "FFmpegReader.h"


FFmpegReader::FFmpegReader()
:m_TimeBase(0)
,m_nBufingTime(0)
,m_nAudioIndex(-1)
,m_nVideoIndex(-1)
,m_strMediaPath(NULL)
{
    ECMemSet(&m_MediaContext, 0, sizeof(MediaContext));
}

FFmpegReader::~FFmpegReader()
{
}

EC_U32 FFmpegReader::OpenMedia(const char* pMediaPath)
{
    av_register_all();
    avformat_network_init();
    m_strMediaPath = pMediaPath;

    //m_MediaContext.pFormatCtx = avformat_alloc_context();
    //m_MediaContext.pFormatCtx->interrupt_callback.opaque = NULL;
    //m_MediaContext.pFormatCtx->interrupt_callback.callback = open_input_interrupt_callback;
    //AVDictionary *optionsDict = NULL;
    //av_dict_set(&optionsDict, "rtsp_transport", "tcp", 0);
    //av_dict_set(&optionsDict, "network_timeout", "60000000", 0);


    int ffmpeg_ret = avformat_open_input(&m_MediaContext.pFormatCtx, m_strMediaPath.ToCStr(), NULL, NULL);
    if (ffmpeg_ret != 0)
    {
        ECLogE("ffmpeg open url: %s failed, return:%d", m_strMediaPath.ToCStr(), ffmpeg_ret);
        return Source_Err_OpenFaild;
    }
    ffmpeg_ret = avformat_find_stream_info(m_MediaContext.pFormatCtx, NULL);
    if (ffmpeg_ret < 0)
    {
        ECLogE("ffmpeg find_stream_info failed, return:%d", ffmpeg_ret);
        return Source_Err_NoStreamInfo;
    }

    m_nAudioIndex = av_find_best_stream(m_MediaContext.pFormatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (m_nAudioIndex >= 0)
    {
        AVCodecParameters *pCodecParam = m_MediaContext.pFormatCtx->streams[m_nAudioIndex]->codecpar;
        AVCodec *pCodec = avcodec_find_decoder(pCodecParam->codec_id);
        AVCodecContext *pAVCodecCtx = avcodec_alloc_context3(pCodec);
        AVStream* pStream = m_MediaContext.pFormatCtx->streams[m_nAudioIndex];
        avcodec_parameters_to_context(pAVCodecCtx, pCodecParam);

        m_TimeBase = av_q2d(pStream->time_base);

        m_MediaContext.hasAudio = true;
        m_MediaContext.nAudioIndex = m_nAudioIndex;
        m_MediaContext.pAudioCodecCtx = pAVCodecCtx;
        m_MediaContext.pAudioDecParam = pCodecParam;
        m_MediaContext.nSampleFormat = pCodecParam->format;
        m_MediaContext.nChannels = pCodecParam->channels;
        m_MediaContext.nSampleSize = pCodecParam->frame_size;
        m_MediaContext.nSampleRate = pCodecParam->sample_rate;
    }
    else
    {
        ECLogW("No AudioTrack...!");
    }

    m_nVideoIndex = av_find_best_stream(m_MediaContext.pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (m_nVideoIndex >= 0)
    {
        AVCodecParameters *pCodecParam = m_MediaContext.pFormatCtx->streams[m_nVideoIndex]->codecpar;
        AVCodec *pCodec = avcodec_find_decoder(pCodecParam->codec_id);
        AVCodecContext *pAVCodecCtx = avcodec_alloc_context3(pCodec);
        AVStream* pStream = m_MediaContext.pFormatCtx->streams[m_nVideoIndex];
        avcodec_parameters_to_context(pAVCodecCtx, pCodecParam);
        if(m_TimeBase == 0)
        {
            m_TimeBase = av_q2d(pStream->time_base);
        }
        m_MediaContext.hasVideo = true;
        m_MediaContext.nVideoIndex = m_nVideoIndex;
        m_MediaContext.pVideoCodecCtx = pAVCodecCtx;
        m_MediaContext.pVideoDecParam = pCodecParam;
        m_MediaContext.nVideoWidth = pCodecParam->width;
        m_MediaContext.nVideoHeight = pCodecParam->height;
    }
    else
    {
        ECLogW("No VideoTrack...!");
    }

    if((m_nVideoIndex < 0) && (m_nAudioIndex))
    {
        m_MediaContext.nDuration = 0;
        ECLogE("No Meida Content (No vudio, No audio)..!");
        return Source_Err_NoMediaTrack;
    }
    else
    {
        m_MediaContext.nDuration = (EC_U32)(m_MediaContext.pFormatCtx->duration / TIME_UNIT);
    }

    return Source_Err_None;
}

EC_U32 FFmpegReader::SetPlaybackPos(EC_U32 nPos)
{
    if(nPos > m_MediaContext.nDuration)
        return Source_Err_SetPosFaild;

    EC_U32 nRet = Source_Err_None;
    int64_t timestamp = (int64_t)nPos * 1000;
    avformat_flush(m_MediaContext.pFormatCtx);
    int ret = av_seek_frame(m_MediaContext.pFormatCtx, -1, timestamp, AVSEEK_FLAG_BACKWARD);
    if (ret < 0)
    {
        nRet = Source_Err_SetPosFaild;
    }
    return nRet;
}

EC_U32 FFmpegReader::ReadPacket(SourcePacket* pPacket)
{
    EC_U32 nRet = Source_Err_None;
    if (pPacket)
    {
        EC_U32 nRetryTimes = 0;
        while (nRetryTimes < MAX_READ_PACKET_RETRY)
        {
            int nReadRet = av_read_frame(m_MediaContext.pFormatCtx, pPacket);
            if (nReadRet >= 0)
            {
                int nTimeStamp = 0;
                if (pPacket->pts != AV_NOPTS_VALUE)
                    nTimeStamp = (int)pPacket->pts;
                else if (pPacket->dts != AV_NOPTS_VALUE)
                    nTimeStamp = (int)pPacket->dts;
                m_nBufingTime = m_TimeBase * nTimeStamp * TIME_UNIT;

                if (pPacket->stream_index == m_nAudioIndex)
                    return Source_Err_ReadAudioPkt;
                else if(pPacket->stream_index == m_nVideoIndex)
                    return Source_Err_ReadVideoPkt;
                else
                    ReleasePacketBuffer(pPacket);
            }
            else if (AVERROR_EOF == nReadRet)
            {
                if(IsLiveMedia())
                {
                    return Source_Err_ReadPacketFaild;
                }
                else
                {
                    nRet = Source_Err_ReadEOS;
                    break;
                }
            }
            else
                nRet = Source_Err_ReadPacketFaild;
            nRetryTimes++;
        }
        if (nRetryTimes >= MAX_READ_PACKET_RETRY)
            nRet = Source_Err_ReadPacketFaild;
    }
    else
        nRet = EC_Err_BadParam;

    return nRet;
}

SourcePacket* FFmpegReader::AllocPacket()
{
    AVPacket *pPkt = (AVPacket *)av_malloc(sizeof(AVPacket));
    av_init_packet(pPkt);
    return pPkt;
}

void FFmpegReader::ReleasePacket(SourcePacket** ppPacket)
{
    if (ppPacket)
    {
        av_free((AVPacket*)(*ppPacket));
        ppPacket = NULL;
    }
}

void FFmpegReader::ReleasePacketBuffer(SourcePacket* pPacket)
{
    if (pPacket)
    {
        av_packet_unref(pPacket);
    }
}

void FFmpegReader::CloseMedia()
{
    m_nAudioIndex = -1;
    m_nVideoIndex = -1;
    avcodec_close(m_MediaContext.pAudioCodecCtx);
    avcodec_close(m_MediaContext.pVideoCodecCtx);
    avformat_close_input(&m_MediaContext.pFormatCtx);
    avformat_free_context(m_MediaContext.pFormatCtx);
    ECMemSet(&m_MediaContext, 0, sizeof(MediaContext));
}

bool FFmpegReader::IsLiveMedia() const
{
    return (m_MediaContext.pFormatCtx->duration <= 0);
}

EC_U32 FFmpegReader::GetBufferingTime()
{
    return m_nBufingTime;
}

MediaContext* FFmpegReader::GetMediaContext()
{
    return &m_MediaContext;
}
