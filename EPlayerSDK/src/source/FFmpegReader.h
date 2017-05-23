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
 * FFmpegReader.h
 * This file for all FFMPEG source interface define.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef FFMPEG_READER_H
#define FFMPEG_READER_H

#include "ECType.h"
#include "ECString.h"

extern "C"
{
#include "libavformat/avformat.h"
};

#define TIME_UNIT              1000
#define MAX_READ_PACKET_RETRY    32

typedef AVPacket SourcePacket;
typedef struct __MediaContext
{
    bool   hasVideo;
    bool   hasAudio;
    EC_U32 nChannels;
    EC_U32 nDuration;
    EC_U32 nAudioIndex;
    EC_U32 nVideoIndex;
    EC_U32 nVideoWidth;
    EC_U32 nVideoHeight;
    EC_U32 nSampleRate;
    EC_U32 nSampleSize;
    EC_U32 nSampleFormat;
    AVFormatContext* pFormatCtx;
    AVCodecContext* pAudioCodecCtx;
    AVCodecContext* pVideoCodecCtx;
    AVCodecParameters* pAudioDecParam;
    AVCodecParameters* pVideoDecParam;
} MediaContext;

class FFmpegReader
{
public:
    FFmpegReader();
    ~FFmpegReader();
    EC_U32 OpenMedia(const char* pMediaPath);
    EC_U32 SetPlaybackPos(EC_U32 nPos);
    EC_U32 ReadPacket(SourcePacket* pPacket);
    static SourcePacket* AllocPacket();
    static void ReleasePacket(SourcePacket** ppPacket);
    static void ReleasePacketBuffer(SourcePacket* pPacket);
    void CloseMedia();
    bool IsLiveMedia() const;
    EC_U32 GetBufferingTime();
    MediaContext* GetMediaContext();

private:
    int m_nAudioIndex;
    int m_nVideoIndex;
    double m_TimeBase;
    EC_U32 m_nBufingTime;
    ECString m_strMediaPath;
    MediaContext m_MediaContext;
};

#endif /* FFMPEG_READER_H */
