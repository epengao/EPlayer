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
 * MediaSource.h
 * This file for all MediaSource interface implement.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#include "ECLog.h"
#include "ECUtil.h"
#include "ECAutoLock.h"
#include "MediaSource.h"
#include "SourceAudioPort.h"
#include "SourceVideoPort.h"


MediaSource::MediaSource()
:m_bEOS(false)
,m_nDuration(0)
,m_nSeekPos(-1)
,m_bRunning(false)
,m_pAudioPort(NULL)
,m_pVideoPort(NULL)
,m_pThreadDriver(NULL)
,m_pFFmpegReader(NULL)
,m_pPacketManager(NULL)
{
    m_pFFmpegReader = new FFmpegReader();
    m_pPacketManager = new PacketManager();
    m_pAudioPort = new SourceAudioPort(this);
    m_pVideoPort = new SourceVideoPort(this);
    m_pThreadDriver = new ECThreadDriver(this);
}

MediaSource::~MediaSource()
{
    if (m_pThreadDriver)
    {
        m_pThreadDriver->Exit();
        delete m_pThreadDriver;
    }
    if (m_pFFmpegReader) delete m_pFFmpegReader;
    if (m_pPacketManager) delete m_pPacketManager;
}

EC_U32 MediaSource::OpenMedia(const char* pMediaPath)
{
    m_bEOS = false;
    EC_U32 nRet = EC_Err_None;
    if (NULL == pMediaPath)
        nRet = EC_Err_BadParam;
    else
    {
        ECAutoLock lock(&m_mtxFFmpeg);
        nRet = m_pFFmpegReader->OpenMedia(pMediaPath);
        if(nRet == EC_Err_None)
        {
            MediaContext *pCtx = m_pFFmpegReader->GetMediaContext();
            m_nDuration = (EC_U32)(pCtx->pFormatCtx->duration / 1000);
        }
    }
    return nRet;
}

void MediaSource::CloseMedia()
{
    m_bEOS = false;
    m_bRunning = false;
    {
        m_pThreadDriver->Pause();
        ECAutoLock lock(&m_mtxFFmpeg);
        m_pFFmpegReader->CloseMedia();
        m_pPacketManager->ClearAll();
    }
}

void MediaSource::Run()
{
    m_bRunning = true;
    m_pThreadDriver->Run();
}

void MediaSource::Pause()
{
    m_bRunning = false;
    m_pThreadDriver->Pause();
}

EC_U32 MediaSource::Seek(EC_U32 nPos)
{
    EC_U32 nRet;
    m_bEOS = false;
    m_bRunning = true;
    m_nSeekPos = nPos;
    {
        ECAutoLock lock(&m_mtxFFmpeg);
        nRet = DoSeek();
    }
    m_nSeekPos = -1;
    return nRet;
}

EC_U32 MediaSource::GetDuration()
{
    return m_nDuration;
}

EC_U32 MediaSource::GetBufferingTime()
{
    return m_pFFmpegReader->GetBufferingTime();
}

MediaContext* MediaSource::GetMediaContext()
{
    return m_pFFmpegReader->GetMediaContext();
}

SourceAudioPort* MediaSource::GetMediaSourceAudioPort()
{
    return m_pAudioPort;
}

SourceVideoPort* MediaSource::GetMediaSourceVideoPort()
{
    return m_pVideoPort;
}

/* Private Methods */
void MediaSource::DoPlay()
{
    ECAutoLock lock(&m_mtxFFmpeg);
    if(m_pPacketManager->IsPacketQueueFull())
    {
        ECSleep(SRC_WAIT_TIME);
        return;
    }

    SourcePacket* pPkt = NULL;
    m_pPacketManager->PopEmptyDataPacket(&pPkt);

    if (pPkt != NULL)
    {
        EC_U32 nRet = m_pFFmpegReader->ReadPacket(pPkt);
        if (nRet == Source_Err_ReadAudioPkt)
        {
            m_pPacketManager->PushAudioDataPacket(pPkt);
        }
        else if (nRet == Source_Err_ReadVideoPkt)
        {
            m_pPacketManager->PushVideoDataPacket(pPkt);
        }
        else if(nRet == Source_Err_ReadEOS)
        {
            m_bEOS = true;
            m_bRunning = false;
        }
        else
        {
            m_pPacketManager->PushEmptyDataPacket(pPkt);
            ECSleep(SRC_WAIT_TIME);
        }
    }
    else
    {
        ECSleep(SRC_WAIT_TIME);
    }
}

EC_U32 MediaSource::DoSeek()
{
    m_pPacketManager->ClearAll();
    EC_U32 nRet = m_pFFmpegReader->SetPlaybackPos(m_nSeekPos);
    if (nRet != Source_Err_None)
    {
        return nRet;
    }
    int nMaxTry = SRC_MAX_SEEK_TRY;
    do
    {
        nMaxTry--;
        if (m_pPacketManager->IsPacketQueueFull())
        {
            break;
        }
        SourcePacket* pPkt = NULL;
        m_pPacketManager->PopEmptyDataPacket(&pPkt);
        if (pPkt != NULL)
        {
            EC_U32 nRet = m_pFFmpegReader->ReadPacket(pPkt);
            if (nRet == Source_Err_ReadAudioPkt)
            {
                m_pPacketManager->PushAudioDataPacket(pPkt);
            }
            else if (nRet == Source_Err_ReadVideoPkt)
            {
                m_pPacketManager->PushVideoDataPacket(pPkt);
            }
            else if (nRet == Source_Err_ReadEOS)
            {
                m_bEOS = true;
                m_bRunning = false;
                break;
            }
        }
    } while (nMaxTry >= 0);
    return nRet;
}

void MediaSource::DoRunning()
{
    if (m_bRunning)
    {
        DoPlay();
    }
    else
    {
        ECSleep(SRC_WAIT_TIME);
    }
    if (m_bEOS)
    {
        m_bRunning = false;
    }
}
