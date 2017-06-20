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
 * This file for all MediaSource interface define.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef MEDIA_SOURCE_H
#define MEDIA_SOURCE_H

#include "ECType.h"
#include "ECMutex.h"
#include "ECSignal.h"
#include "FFmpegReader.h"
#include "PacketManager.h"
#include "ECThreadDriver.h"

#define Source_Err_None            0x00000000
#define Source_Err_ReadEOS         0x0000F001
#define Source_Err_OpenFaild       0x0000F002
#define Source_Err_SetPosFaild     0x0000F003
#define Source_Err_ReadAudioPkt    0x0000F004
#define Source_Err_ReadVideoPkt    0x0000F005
#define Source_Err_NoStreamInfo    0x0000F006
#define Source_Err_NoMediaTrack    0x0000F007
#define Source_Err_ReadPacketFaild 0x0000F008

#define SRC_WAIT_TIME               5
#define MAX_NOTIFY_WAIT          1500
#define SRC_MAX_SEEK_TRY (512+256+100)

class SourceAudioPort;
class SourceVideoPort;

class MediaSource : public ECThreadWorkerI
{
public:
    MediaSource();
    ~MediaSource();
    EC_U32 OpenMedia(const char* pMediaPath);
    void CloseMedia();
    void Run();
    void Pause();
    EC_U32 Seek(EC_U32 nPos);
    EC_U32 GetDuration();
    EC_U32 GetBufferingTime();
    MediaContext* GetMediaContext();
    SourceAudioPort* GetMediaSourceAudioPort();
    SourceVideoPort* GetMediaSourceVideoPort();

private:
    void DoPlay();
    EC_U32 DoSeek();
    void CheckBufferting(EC_U32 nCheckRet);
    /* ECThreadWorkerI */
    virtual void DoRunning();

private:
    bool               m_bEOS;
    bool               m_bBuffing;
    bool               m_bRunning;
    int                m_nSeekPos;
    EC_U32             m_nDuration;
    ECMutex            m_mtxFFmpeg;
    SourceAudioPort*   m_pAudioPort;
    SourceVideoPort*   m_pVideoPort;
    FFmpegReader*      m_pFFmpegReader;
    PacketManager*     m_pPacketManager;
    ECThreadDriver*    m_pThreadDriver;
    EC_U32             m_nLastReadPktFailTime;

    friend class SourceAudioPort;
    friend class SourceVideoPort;
};

#endif /* MEDIA_SOURCE_H */
