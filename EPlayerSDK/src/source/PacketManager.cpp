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
 * PacketManager.cpp
 * This file for all A/V PacketManager interface implementation.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#include "ECAutoLock.h"
#include "PacketManager.h"

PacketManager::PacketManager()
{
	EC_U32 aPktQSize = A_PACKET_QUEUE_MAX_SIZE;
	EC_U32 vPktQSize = V_PACKET_QUEUE_MAX_SIZE;
    m_pAudioDataPacketQueue = new ECRingQueue<SourcePacket*>(aPktQSize);
	m_pVideoDataPacketQueue = new ECRingQueue<SourcePacket*>(vPktQSize);
	m_pEmptyDataPacketQueue = new ECRingQueue<SourcePacket*>(aPktQSize + vPktQSize);

	for (EC_U32 i = 0; i < aPktQSize + vPktQSize; i++)
    {
        m_pEmptyDataPacketQueue->Push(FFmpegReader::AllocPacket());
    }
}

PacketManager::~PacketManager()
{
    SourcePacket *pPkt = NULL;
    while (EC_Err_None == m_pEmptyDataPacketQueue->Pop(&pPkt))
    {
        if (pPkt)
        {
            FFmpegReader::ReleasePacket(&pPkt);
        }
    }
    while (EC_Err_None == m_pAudioDataPacketQueue->Pop(&pPkt))
    {
        if (pPkt)
        {
            FFmpegReader::ReleasePacketBuffer(pPkt);
            FFmpegReader::ReleasePacket(&pPkt);
        }
    }
    while (EC_Err_None == m_pVideoDataPacketQueue->Pop(&pPkt))
    {
        if (pPkt)
        {
            FFmpegReader::ReleasePacketBuffer(pPkt);
            FFmpegReader::ReleasePacket(&pPkt);
        }
    }
    if (m_pAudioDataPacketQueue) delete m_pAudioDataPacketQueue;
    if (m_pVideoDataPacketQueue) delete m_pVideoDataPacketQueue;
    if (m_pEmptyDataPacketQueue) delete m_pEmptyDataPacketQueue;
}

void PacketManager::ClearAll()
{
    ECAutoLock autoLock(&m_mtxEmptyDataPacket);
    SourcePacket *pAudioPkt = NULL;
    {
        ECAutoLock Lock(&m_mtxAudioDataPacket);
        while (EC_Err_None == m_pAudioDataPacketQueue->Pop(&pAudioPkt))
        {
            if (pAudioPkt)
            {
                FFmpegReader::ReleasePacketBuffer(pAudioPkt);
                m_pEmptyDataPacketQueue->Push(pAudioPkt);
            }
        }
    }
    SourcePacket *pVideoPkt = NULL;
    {
        ECAutoLock Lock(&m_mtxVideoDataPacket);
        while (EC_Err_None == m_pVideoDataPacketQueue->Pop(&pVideoPkt))
        {
            if (pVideoPkt)
            {
                FFmpegReader::ReleasePacketBuffer(pVideoPkt);
                m_pEmptyDataPacketQueue->Push(pVideoPkt);
            }
        }
    }
}

bool PacketManager::IsPacketQueueFull() const
{
    bool audioPktFull = m_pAudioDataPacketQueue->IsFull();
    bool videoPktFull = m_pVideoDataPacketQueue->IsFull();
    return (audioPktFull || videoPktFull);
}

/* Empty Packet */
EC_U32 PacketManager::PushEmptyDataPacket(SourcePacket *pPacket)
{
    if (pPacket)
    {
        ECAutoLock Lock(&m_mtxEmptyDataPacket);
        return m_pEmptyDataPacketQueue->Push(pPacket);
    }
    else
        return EC_Err_BadParam;
}

EC_U32 PacketManager::PopEmptyDataPacket(SourcePacket **ppPacket)
{
    if (ppPacket)
    {
        ECAutoLock Lock(&m_mtxEmptyDataPacket);
        return m_pEmptyDataPacketQueue->Pop(ppPacket);
    }
    else
        return EC_Err_BadParam;
}

/* Audio Packet Interface */
EC_U32 PacketManager::PushAudioDataPacket(SourcePacket *pPacket)
{
    if (pPacket)
    {
        ECAutoLock Lock(&m_mtxAudioDataPacket);
        return m_pAudioDataPacketQueue->Push(pPacket);
    }
    else
        return EC_Err_BadParam;
}

EC_U32 PacketManager::PopAudioDataPacket(SourcePacket **ppPacket)
{
    if (ppPacket)
    {
        ECAutoLock Lock(&m_mtxAudioDataPacket);
        return m_pAudioDataPacketQueue->Pop(ppPacket);
    }
    else
        return EC_Err_BadParam;
}

/* Video Packet Interface */
EC_U32 PacketManager::PushVideoDataPacket(SourcePacket *pPacket)
{
    if (pPacket)
    {
        ECAutoLock Lock(&m_mtxVideoDataPacket);
        return m_pVideoDataPacketQueue->Push(pPacket);
    }
    else
        return EC_Err_BadParam;
}

EC_U32 PacketManager::PopVideoDataPacket(SourcePacket **ppPacket)
{
    if (ppPacket)
    {
        ECAutoLock Lock(&m_mtxVideoDataPacket);
        return m_pVideoDataPacketQueue->Pop(ppPacket);
    }
    else
        return EC_Err_BadParam;
}
