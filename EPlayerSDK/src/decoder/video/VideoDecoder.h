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
 * VideoDecoder.h
 * This file for Media video decoder module class define.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef VIDEO_DECODER_H
#define VIDEO_DECODER_H

#include "ECMutex.h"
#include "ECThreadDriver.h"
#include "SourceVideoPort.h"

typedef struct
{
    AVFrame* pAVFrame;
    EC_U64   nTimestamp;
} VideoFrame;

#define VIDEO_DEC_WAIT_TIME  10

class VideoDecPort;
class FFmpegVideoDecoder;

class VideoDecoder
{
public:
    VideoDecoder();
    ~VideoDecoder();

    EC_U32 OpenDecoder(MediaContext* pMediaContext,
                       SourceVideoPort* pSourcePort);
    void CloseDecoder();
    void Flush();
    EC_U32 GetVideoFrame(VideoFrame* pFrameOut);
    VideoDecPort* GetVideoDecPort() { return m_pPort; };

private:
    ECMutex             m_mtxDec;
    SourcePacket*       m_pCurrPacket;
    VideoDecPort*       m_pPort;
    SourceVideoPort*    m_pSourcePort;
    FFmpegVideoDecoder* m_pFFmpegDecoder;
};

#endif /* VIDEO_DECODER_H */
