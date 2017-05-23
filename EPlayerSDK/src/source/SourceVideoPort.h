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
 * SourceVideoPort.h
 * This file define a Port, for other moduels get source video Pkt.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef SOURCE_VIDEO_PORT_H
#define SOURCE_VIDEO_PORT_H

#include "MediaSource.h"

class SourceVideoPort
{
public:
    SourceVideoPort(MediaSource* pOwner);
    ~SourceVideoPort();

    EC_U32 GetVideoDataPacket(SourcePacket** ppPacket);
    EC_U32 ReturnVideoEmptyPacket(SourcePacket* pPacket);
private:
    MediaSource*    m_pOwner;
};

#endif /* SOURCE_VIDEO_PORT_H */
