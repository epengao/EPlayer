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
 * SDL2_AudioDevice.cpp
 * This file for SDL2 audio sound output device
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#include "ECUtil.h"
#include "ECMemOP.h"
#include "AudioRender.h"
#include "SDL2_AudioDevice.h"

SDL2_AudioDevice::SDL2_AudioDevice(AudioRender* pAudioRender)
:AudioDeviceI(pAudioRender)
,m_pMutePCM(NULL)
,m_pResample(NULL)
,m_nMuteBufSize(MUTE_SOUND_BUF_SIZE)
{
	m_pMutePCM = (EC_U8*)ECMemAlloc(m_nMuteBufSize);
	ECMemSet(m_pMutePCM, 0, m_nMuteBufSize);
    SDL_memset(&m_sAudioContext, 0, sizeof(m_sAudioContext));
}

SDL2_AudioDevice::~SDL2_AudioDevice()
{
	if (m_pMutePCM) ECMemFree(m_pMutePCM);
}

int SDL2_AudioDevice::Init(MediaContext* pMediaCtx)
{
    m_sAudioContext.silence = 0;
    m_sAudioContext.userdata = this;
    m_sAudioContext.callback = PlaySoundCallback;
    m_sAudioContext.freq = pMediaCtx->nSampleRate;
    m_sAudioContext.channels = pMediaCtx->nChannels;
    m_sAudioContext.samples = pMediaCtx->nSampleSize;
    AudioSampleFormat nSampleFmt = m_pAudioRender->SampleFmtSwitch(pMediaCtx->nSampleFormat);
    m_sAudioContext.format = SampleFmtSwitch(nSampleFmt);

    if (SDL_Init(SDL_INIT_AUDIO) < 0)
        return Audio_Render_Err_DeviceErr;
    if (SDL_OpenAudio(&m_sAudioContext, NULL) < 0)
        return Audio_Render_Err_DeviceErr;

    return Audio_Render_Err_None;
}

void SDL2_AudioDevice::Uninit()
{
    SDL_CloseAudio();
}

void SDL2_AudioDevice::Run()
{
    SDL_PauseAudio(0);
}

void SDL2_AudioDevice::Pause()
{
    SDL_PauseAudio(1);
}

int SDL2_AudioDevice::SampleFmtSwitch(AudioSampleFormat nFmtIn)
{
    switch (nFmtIn)
    {
    case AudioSampleFormat_U8:
    case AudioSampleFormat_U8P:    return AUDIO_U8;
    case AudioSampleFormat_S16:
    case AudioSampleFormat_S16P:   return AUDIO_S16SYS;
    case AudioSampleFormat_S32:
    case AudioSampleFormat_S32P:   return AUDIO_S32SYS;
    case AudioSampleFormat_Float:
    case AudioSampleFormat_Double:
    case AudioSampleFormat_FloatP: return AUDIO_F32SYS;
    default: return AUDIO_U16SYS;
    }
}

void SDL2_AudioDevice::PlaySoundCallback(void* pUserData, Uint8* pStream, int nLen)
{
    char* pPCM = NULL;
    unsigned int nOutSize = 0;
    unsigned int nOutSamples = 0;

    SDL2_AudioDevice *pSelf = (SDL2_AudioDevice*)pUserData;
    pSelf->m_pAudioRender->GetPCMBuffer(&pPCM, &nOutSize, &nOutSamples);

    SDL_memset(pStream, 0, nLen);
    if (pPCM && (nOutSize > 0))
    {
        SDL_MixAudio(pStream, (Uint8*)pPCM, nOutSize, SDL_MIX_MAXVOLUME);
    }
    else
    {
        SDL_MixAudio(pStream, (Uint8*)pSelf->m_pMutePCM, pSelf->m_nMuteBufSize, SDL_MIX_MAXVOLUME);
    }
}
