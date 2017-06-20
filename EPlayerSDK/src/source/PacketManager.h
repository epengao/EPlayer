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
 * PacketManager.h
 * This file for all Audio/Video PacketManager interface define.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef PACKET_MANAGER_H
#define PACKET_MANAGER_H

#include "ECMutex.h"
#include "ECRingQueue.h"
#include "FFmpegReader.h"

#define A_PACKET_QUEUE_MAX_SIZE 256
#define V_PACKET_QUEUE_MAX_SIZE 512

class PacketManager
{
public:
    PacketManager();
    ~PacketManager();

    void ClearAll();
    bool IsPacketQueueFull() const;
    bool IsPacketQueueEmpty() const;
    /* Empty Packet Interface */
    EC_U32 PushEmptyDataPacket(SourcePacket *pPacket);
    EC_U32 PopEmptyDataPacket(SourcePacket **ppPacket);
    /* Audio Packet Interface */
    EC_U32 PushAudioDataPacket(SourcePacket *pPacket);
    EC_U32 PopAudioDataPacket(SourcePacket **ppPacket);
    /* Video Packet Interface */
    EC_U32 PushVideoDataPacket(SourcePacket *pPacket);
    EC_U32 PopVideoDataPacket(SourcePacket **ppPacket);

private:
    /* Empty Packet Queue */
    ECMutex                     m_mtxEmptyDataPacket;
    ECRingQueue<SourcePacket*>* m_pEmptyDataPacketQueue;
    /* Auido Packet Queue */
    ECMutex                     m_mtxAudioDataPacket;
    ECRingQueue<SourcePacket*>* m_pAudioDataPacketQueue;
    /* Video Packet Queue */
    ECMutex                     m_mtxVideoDataPacket;
    ECRingQueue<SourcePacket*>* m_pVideoDataPacketQueue;
};

#endif /* PACKET_MANAGER_H */
