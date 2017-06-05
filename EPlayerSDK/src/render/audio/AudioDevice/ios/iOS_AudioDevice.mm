/*
* This software is developed for study and improve coding skill ...
*
* Project:  Enjoyable Coding< EC >
* Copyright (C) 2014-2016 Gao Peng

* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Library General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.

* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.

* You should have received a copy of the GNU Library General Public
* License along with this library; if not, write to the Free
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

* ---------------------------------------------------------------------
* SDL_AudioRender.h
* This file is SDL_AudioRender class implementation
*
* Eamil:   epengao@126.com
* Author:  Peter Gao
* Version: Intial first version.
* --------------------------------------------------------------------
*/

#include "ECMemOP.h"
#include "ECOSUtil.h"
#include "ECAutoLock.h"
#include "iOS_AudioRender.h"
extern "C"
{
#include "libavformat/avformat.h"
};

iOS_AudioRender::iOS_AudioRender()
:m_nCurrFillBufIndex(0)
,m_bNeedFillBufQueue(EC_TRUE)
,m_nStatus(MediaEngStatus_UnDefine)
,m_bRndCallbackFinished(EC_TRUE)
,m_semFillPCMBufSignal(0, 1)
,m_semRendStopedSignal(0, 1)
,m_semRendFinishedSignal(0, 1)
{
    ecMemSet(&m_sAudioDescription, 0, sizeof(m_sAudioDescription));
}

iOS_AudioRender::~iOS_AudioRender()
{
}

EC_U32 iOS_AudioRender::Init(MediaCtxInfo* pMediaInfo,
                             AudioPCMBuffer *pFirstFrame)
{
    if (EC_NULL == pMediaInfo)
        return EC_Err_BadParam;

    m_sAudioDescription.mSampleRate = pMediaInfo->m_nSampleRate;;
    m_sAudioDescription.mFormatID = kAudioFormatLinearPCM;
    m_sAudioDescription.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    m_sAudioDescription.mChannelsPerFrame = 2;
    m_sAudioDescription.mFramesPerPacket = 1;
    m_sAudioDescription.mBitsPerChannel = 16;
    m_sAudioDescription.mBytesPerFrame =
        m_sAudioDescription.mBitsPerChannel / 8 * m_sAudioDescription.mChannelsPerFrame;
    m_sAudioDescription.mBytesPerPacket =
        m_sAudioDescription.mFramesPerPacket * m_sAudioDescription.mBytesPerFrame;
    AudioQueueNewOutput(&m_sAudioDescription, PlaySoundCallback, this, nil, nil, 0, &m_rAudioQueue);

    for(int i=0;i<QUEUE_BUFFER_COUNT;i++)
    {
        AudioQueueAllocateBuffer(m_rAudioQueue, MAX_BUFFER_SIZE, &m_sAudioQueueBuffers[i]);
    }

    return Audio_Render_Err_None;
}

EC_VOID iOS_AudioRender::Uninit()
{
    ecMemSet(&m_sAudioDescription, 0, sizeof(m_sAudioDescription));
    for(int i=0;i<QUEUE_BUFFER_COUNT;i++)
    {
        AudioQueueFreeBuffer(m_rAudioQueue, m_sAudioQueueBuffers[i]);
    }
}

EC_VOID iOS_AudioRender::Run()
{
    AudioQueueStart(m_rAudioQueue, NULL);
    m_nStatus = MediaEngStatus_Playing;
}

EC_VOID iOS_AudioRender::Pause()
{
    AudioQueuePause(m_rAudioQueue);
    m_nStatus = MediaEngStatus_Pause;
}

EC_VOID iOS_AudioRender::Stop()
{
    if(m_nStatus == MediaEngStatus_Stop)
    {
        return;
    }
    m_nStatus = MediaEngStatus_Stop;
    m_semFillPCMBufSignal.SendSIGNAL();
    m_semRendFinishedSignal.SendSIGNAL();
    AudioQueueStop(m_rAudioQueue, YES);
}

EC_VOID iOS_AudioRender::Flush()
{
    m_nStatus = MediaEngStatus_Seeking;
    if(!m_bRndCallbackFinished)
    {
        m_semRendFinishedSignal.SendSIGNAL();
    }
    
    AudioQueueReset(m_rAudioQueue);
    m_nCurrFillBufIndex = 0;
    m_bNeedFillBufQueue = EC_TRUE;
}

EC_VOID iOS_AudioRender::SendPCMData(AudioPCMBuffer *pBuffer, EC_U32 nSize)
{
    if (pBuffer && (nSize > 0))
    {
        if(m_bNeedFillBufQueue)
        {
            AudioQueueBufferRef pCurBuf = m_sAudioQueueBuffers[m_nCurrFillBufIndex];
            pCurBuf->mAudioDataByteSize = nSize;
            Byte *pRndBuf = (Byte*)pCurBuf->mAudioData;
            memcpy(pRndBuf, pBuffer, nSize);
            AudioQueueEnqueueBuffer(m_rAudioQueue, pCurBuf, 0, NULL);
            m_nCurrFillBufIndex++;
            if(m_nCurrFillBufIndex >= QUEUE_BUFFER_COUNT) m_bNeedFillBufQueue = EC_FALSE;
            return;
        }
        else
        {
            m_semRendFinishedSignal.WaitSIGNAL();
            Byte *pRndBuf = (Byte*)m_sCurrAudioQueueBuf->mAudioData;
            memcpy(pRndBuf, pBuffer, nSize);
            m_sCurrAudioQueueBuf->mAudioDataByteSize = nSize;
            AudioQueueEnqueueBuffer(m_rAudioQueue, m_sCurrAudioQueueBuf, 0, NULL);
        }
        m_semFillPCMBufSignal.SendSIGNAL();
    }
}

EC_VOID iOS_AudioRender::PlaySoundCallback(EC_VOID *pUserData,
                                           AudioQueueRef inQueue,
                                           AudioQueueBufferRef outQueueBuf)
{
    iOS_AudioRender *pSelf = (iOS_AudioRender*)pUserData;
    pSelf->m_bRndCallbackFinished = EC_FALSE;
    if(MediaEngStatus_Stop == pSelf->m_nStatus)
    {
        return;
    }
    pSelf->m_sCurrAudioQueueBuf = outQueueBuf;
    pSelf->m_semRendFinishedSignal.SendSIGNAL();
    pSelf->m_semFillPCMBufSignal.WaitSIGNAL();
    pSelf->m_bRndCallbackFinished = EC_TRUE;
}
