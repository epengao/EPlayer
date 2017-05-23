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
 * FFmpegVideoDecoder.h
 * This file for video FFMPEG decoder interface define.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef FFMPEG_VIDEO_DECODER_H
#define FFMPEG_VIDEO_DECODER_H

#include "ECType.h"
#include "VideoDecoder.h"
#include "MediaSource.h"

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
};

#define Video_Dec_Err_None            0x00000000
#define Video_Dec_Err_ReadEOS         0x000F1001
#define Video_Dec_Err_OpenFaild       0x000F1002
#define Video_Dec_Err_SkipPkt         0x000F1003
#define Video_Dec_Err_KeepPkt         0x000F1004
#define Video_Dec_Err_NeedNewPkt      0x000F1005
#define Video_Dec_Err_DecFrameFaild   0x000F1007

class FFmpegVideoDecoder
{
public:
    FFmpegVideoDecoder();
    ~FFmpegVideoDecoder();
    EC_U32 OpenDecoder(MediaContext* pMediaContext);
    void CloseDecoder();
    void FlushDecoder();
    EC_U32 SetInputPacket(SourcePacket* pInputPacket);
    EC_U32 GetOutputFrame(VideoFrame* pOutputVideoFrame);

private:
    AVFrame*           m_pFrame;
    AVCodecContext*    m_pCodecCtx;
    AVCodecParameters* m_pCodecParam;
    double             m_TimeBase;
    EC_U32             m_nMaxPCMBufSize;
};

#endif /* FFMPEG_VIDEO_DECODER_H */
