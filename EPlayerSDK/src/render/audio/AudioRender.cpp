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
 * AudioRender.cpp
 * This file for Media Audio Render class APIs impelement.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#include "ECLog.h"
#include "ECUtil.h"
#include "ECOSDef.h"
#include "ECMemOP.h"
#include "ECAutoLock.h"
#include "MessageHub.h"
#include "AudioRender.h"
#include "AudioDeviceI.h"
#include "AudioDecPort.h"

#define MAX_SEEK_TRY        500
#define MAX_NOTIFY_WAIT    1000
#define A_RND_SEEK_MAX_TRY  300

#ifdef EC_OS_Win32
#include "Win_AudioDevice.h"
#elif defined EC_OS_Linux
/* TODO */
#elif defined EC_OS_MacOS
#include "Mac_AudioDevice.h"
#elif defined EC_OS_iOS
#include "iOS_AudioDevice.h"
#elif defined EC_OS_Android
#include "Android_AudioDevice.h"
#endif

AudioRender::AudioRender(MediaClock* pClock)
:m_bEOS(false)
,m_bRawPCM(false)
,m_bBufing(false)
,m_bRunning(false)
,m_nLastRndTime(0)
,m_pDecoderPort(NULL)
,m_pAudioDevice(NULL)
,m_pMedaiClock(pClock)
{
#ifdef EC_OS_Win32
    m_pAudioDevice = new Win_AudioDevice(this);
#elif defined EC_OS_Linux
    /* TODO */
#elif defined EC_OS_MacOS
    m_pAudioDevice = new Mac_AudioDevice(this);
#elif defined EC_OS_iOS
    m_pAudioDevice = new iOS_AudioDevice(this);
#elif defined EC_OS_Android
    m_pAudioDevice = new Android_AudioDevice(this);
#endif
}

AudioRender::~AudioRender()
{
    if (m_pAudioDevice)
    {
        delete m_pAudioDevice;
    }
}

void AudioRender::Run()
{
    m_pAudioDevice->Run();
}

void AudioRender::Pause()
{
    m_pAudioDevice->Pause();
    m_bRunning = false;
}

void AudioRender::Flush()
{
    m_pAudioDevice->Flush();
}

void AudioRender::Seek(EC_U32 nPos, bool fastSeek)
{
    Flush();

    EC_U32 nRet = 0;
    AudioFrame audioFram;
    bool checkAgain = false;
    EC_U32 nMaxTry = A_RND_SEEK_MAX_TRY;
    do
    {
        nMaxTry--;
        audioFram.nSamples = 0;
        audioFram.nDataSize = 0;
        audioFram.nTimestamp = 0;
        audioFram.pPCMData = NULL;
        nRet = m_pDecoderPort->GetAudioFrame(&audioFram, m_bRawPCM);
        if(fastSeek)
        {
            checkAgain = (nRet != Audio_Dec_Err_None);
        }
        else
        {
            checkAgain = (audioFram.nTimestamp < nPos);
        }
    } while (nMaxTry > 0 && checkAgain);

    if(nRet == Audio_Dec_Err_None)
    {
        m_pMedaiClock->SetMediaTime(audioFram.nTimestamp);
    }
}

EC_U32 AudioRender::OpenDevice(MediaContext* pMediaContext,
                               AudioDecPort* pAudioDecPort)
{
    m_bEOS = false;
    m_bBufing = false;
    m_bRunning = false;
    m_pDecoderPort = pAudioDecPort;
    return m_pAudioDevice->Init(pMediaContext);
}

void AudioRender::CloseDevice()
{
    m_bEOS = false;
    m_bBufing = false;
    m_bRunning = false;
    m_pAudioDevice->Uninit();
}

void AudioRender::GetPCMBuffer(char** ppOutBuf, EC_U32* pOutSize, EC_U32* pOutSamples, bool rawData)
{
    if (ppOutBuf && pOutSize)
    {
        EC_U32 nRet;
		m_bRawPCM = rawData;
        EC_U32 nTryTimes = 0;
        AudioFrame audioFram;
        do
        {
            audioFram.nSamples = 0;
            audioFram.nDataSize = 0;
            audioFram.nTimestamp = 0;
            audioFram.pPCMData = NULL;

            nRet = m_pDecoderPort->GetAudioFrame(&audioFram, rawData);
            if(Audio_Dec_Err_None == nRet)
            {
                (*ppOutBuf) = audioFram.pPCMData;
                (*pOutSize) = audioFram.nDataSize;
                (*pOutSamples) = audioFram.nSamples;
                m_nLastRndTime = ECGetSystemTime();
                if(m_bBufing)
                {
                    m_bBufing = false;
                    MessageHub* pMsgHub = MessageHub::GetInstance();
                    pMsgHub->SendMessage(PlayerMessage_Audio_BufferingStop);
                }
                if(!m_bRunning)
                {
                    m_bRunning = true;
                    MessageHub* pMsgHub = MessageHub::GetInstance();
                    pMsgHub->SendMessage(PlayerMessage_PlayStart);
                }
                if(m_pMedaiClock)
                {
                    m_pMedaiClock->UpdateClockTime(audioFram.nTimestamp);
                }
                return;
            }
            else if(Source_Err_ReadEOS == nRet)
            {
                if(!m_bEOS)
                {
                    m_bEOS = true;
                    MessageHub* pMsgHub = MessageHub::GetInstance();
                    pMsgHub->SendMessage(PlayerMessage_Audio_PlayFinished);
                }
                return;
            }
            else
            {
                nTryTimes++;
            }
        }
        while (nTryTimes < MAX_SEEK_TRY);
    }

    if( (!m_bBufing) && (ECGetSystemTime() - m_nLastRndTime > MAX_NOTIFY_WAIT) )
    {
        m_bBufing = true;
        MessageHub* pMsgHub = MessageHub::GetInstance();
        pMsgHub->SendMessage(PlayerMessage_Audio_BufferingStart);
    }
}

AudioSampleFormat AudioRender::SampleFmtSwitch(int nFmtIn)
{
    switch (nFmtIn)
    {
        case AV_SAMPLE_FMT_U8:   return AudioSampleFormat_U8;
        case AV_SAMPLE_FMT_S16:  return AudioSampleFormat_S16;
        case AV_SAMPLE_FMT_S32:  return AudioSampleFormat_S32;
        case AV_SAMPLE_FMT_FLT:  return AudioSampleFormat_Float;
        case AV_SAMPLE_FMT_DBL:  return AudioSampleFormat_Double;
        case AV_SAMPLE_FMT_U8P:  return AudioSampleFormat_U8P;
        case AV_SAMPLE_FMT_S16P: return AudioSampleFormat_S16P;
        case AV_SAMPLE_FMT_S32P: return AudioSampleFormat_S32P;
        case AV_SAMPLE_FMT_FLTP: return AudioSampleFormat_FloatP;
        case AV_SAMPLE_FMT_DBLP: return AudioSampleFormat_DoubleP;
        case AV_SAMPLE_FMT_S64:  return AudioSampleFormat_S64;
        case AV_SAMPLE_FMT_S64P: return AudioSampleFormat_S64P;
        case AV_SAMPLE_FMT_NB:   return AudioSampleFormat_NB;
    }
    return AudioSampleFormat_Error;
}
