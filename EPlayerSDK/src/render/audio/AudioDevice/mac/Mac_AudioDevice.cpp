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
 * Mac_AudioDevice.cpp
 * This file is Mac_AudioDevice class implementation
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#include "ECMemOP.h"
#include "Mac_AudioDevice.h"

Mac_AudioDevice::Mac_AudioDevice(AudioRender* pAudioRender)
:AudioDeviceI(pAudioRender)
{
    ECMemSet(&m_sAudioDescription, 0, sizeof(m_sAudioDescription));
}

int Mac_AudioDevice::Init(MediaContext* pMediaCtx)
{
    AudioSampleFormat nSampleFmt = m_pAudioRender->SampleFmtSwitch(pMediaCtx->nSampleFormat);
    m_sAudioDescription.mFramesPerPacket = 1;
    m_sAudioDescription.mSampleRate = pMediaCtx->nSampleRate;
    m_sAudioDescription.mChannelsPerFrame = pMediaCtx->nChannels;
    m_sAudioDescription.mFormatID = kAudioFormatLinearPCM;
    m_sAudioDescription.mFormatFlags = SampleFmtSwitch(nSampleFmt);
    m_sAudioDescription.mBitsPerChannel = SmpleBitSwitch(nSampleFmt);
    m_sAudioDescription.mBytesPerFrame = m_sAudioDescription.mBitsPerChannel / 8 * m_sAudioDescription.mChannelsPerFrame;
    m_sAudioDescription.mBytesPerPacket = m_sAudioDescription.mFramesPerPacket * m_sAudioDescription.mBytesPerFrame;

    AudioQueueNewOutput(&m_sAudioDescription, PlaySoundCallback, this, nil, nil, 0, &m_rAudioQueue);
    for(int i=0;i<QUEUE_BUFFER_COUNT;i++)
    {
        AudioQueueAllocateBuffer(m_rAudioQueue, MAX_BUFFER_SIZE, &m_sAudioQueueBuffers[i]);
    }
    for(int i = 0; i < QUEUE_BUFFER_COUNT; i++)
    {
        AudioQueueBufferRef aRef = m_sAudioQueueBuffers[i];
        aRef->mAudioDataByteSize = MUTE_SOUND_SIZE;
        ECMemSet(aRef->mAudioData, 0, MUTE_SOUND_SIZE);
        AudioQueueEnqueueBuffer(m_rAudioQueue, aRef, 0, NULL);
    }
    return Audio_Render_Err_None;
}

void Mac_AudioDevice::Uninit()
{
    ECMemSet(&m_sAudioDescription, 0, sizeof(m_sAudioDescription));
    for(int i=0;i<QUEUE_BUFFER_COUNT;i++)
    {
        AudioQueueFreeBuffer(m_rAudioQueue, m_sAudioQueueBuffers[i]);
    }
}

void Mac_AudioDevice::Run()
{
    AudioQueueStart(m_rAudioQueue, NULL);
}

void Mac_AudioDevice::Pause()
{
    AudioQueuePause(m_rAudioQueue);
}

void Mac_AudioDevice::Flush()
{
    AudioQueueReset(m_rAudioQueue);
    for(int i = 0; i < QUEUE_BUFFER_COUNT; i++)
    {
        AudioQueueBufferRef aRef = m_sAudioQueueBuffers[i];
        aRef->mAudioDataByteSize = 1;
        ECMemSet(aRef->mAudioData, 0, 1);
        AudioQueueEnqueueBuffer(m_rAudioQueue, aRef, 0, NULL);
    }
}

UInt32 Mac_AudioDevice::SmpleBitSwitch(AudioSampleFormat nFmtIn)
{
    switch (nFmtIn)
    {
        case AudioSampleFormat_U8:
        case AudioSampleFormat_U8P:    return 8;
        case AudioSampleFormat_S16:
        case AudioSampleFormat_S16P:   return 16;
        case AudioSampleFormat_S32:
        case AudioSampleFormat_S32P:   return 32;
        case AudioSampleFormat_Float:
        case AudioSampleFormat_Double:
        case AudioSampleFormat_FloatP: return 32;
        default: return 32;
    }
}

UInt32 Mac_AudioDevice::SampleFmtSwitch(AudioSampleFormat nFmtIn)
{
    switch (nFmtIn)
    {
        case AudioSampleFormat_U8:
        case AudioSampleFormat_U8P:
            return kAppleLosslessFormatFlag_16BitSourceData | kAudioFormatFlagIsPacked;
        case AudioSampleFormat_S16:
        case AudioSampleFormat_S16P:
            return kLinearPCMFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
        case AudioSampleFormat_S32:
        case AudioSampleFormat_S32P:
            return kLinearPCMFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
        case AudioSampleFormat_Float:
        case AudioSampleFormat_FloatP:
        case AudioSampleFormat_Double:
        case AudioSampleFormat_DoubleP:
            return kLinearPCMFormatFlagIsFloat | kAudioFormatFlagIsPacked;
        default:
            return kLinearPCMFormatFlagIsFloat | kAudioFormatFlagIsPacked;
    }
}

void Mac_AudioDevice::PlaySoundCallback(void* pUserData, AudioQueueRef inQueue, AudioQueueBufferRef outQueueBuf)
{
    char* pPCM = NULL;
    unsigned int nOutSize = 0;
    unsigned int nOutSamples = 0;
    Mac_AudioDevice *pSelf = (Mac_AudioDevice*)pUserData;

    pSelf->m_pAudioRender->GetPCMBuffer(&pPCM, &nOutSize, &nOutSamples);
    Byte *pAudioQueueRndBuf = (Byte*)outQueueBuf->mAudioData;
    if (pPCM && (nOutSize > 0))
    {
        ECMemCopy(pAudioQueueRndBuf, pPCM, nOutSize);
    }
    else
    {
        nOutSize = WAIT_BUF_SOUND_SIZE;
        ECMemSet(pAudioQueueRndBuf, 0, nOutSize);
    }
    outQueueBuf->mAudioDataByteSize = nOutSize;
    AudioQueueEnqueueBuffer(pSelf->m_rAudioQueue, outQueueBuf, 0, NULL);
}
