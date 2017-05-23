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
 * VideoDecPort.h
 * This file define a Port, for other moduels get Video Frame data.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef VIDEO_DEC_PORT_H
#define VIDEO_DEC_PORT_H

#include "ECType.h"
#include "VideoDecoder.h"

class VideoDecPort
{
public:
    VideoDecPort(VideoDecoder *pDecoder);
    ~VideoDecPort();
    EC_U32 GetVideoFrame(VideoFrame* pFrameOut);

private:
    VideoDecoder*    m_pOwner;
};

#endif /* VIDEO_DEC_PORT_H */
