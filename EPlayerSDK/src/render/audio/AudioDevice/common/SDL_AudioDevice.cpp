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
 * SDL_AudioDevice.cpp
 * This file for SDL audio sound output device
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#include "ECUtil.h"
#include "ECMemOP.h"
#include "AudioRender.h"
#include "SDL_AudioDevice.h"

SDL_AudioDevice::SDL_AudioDevice(AudioRender* pAudioRender)
:AudioDeviceI(pAudioRender)
,m_pMutePCM(NULL)
,m_pResample(NULL)
,m_nMuteBufSize(MUTE_SOUND_BUF_SIZE)
{
    m_pMutePCM = (EC_U8*)ECMemAlloc(m_nMuteBufSize);
    ECMemSet(m_pMutePCM, 0, m_nMuteBufSize);
    SDL_memset(&m_sAudioContext, 0, sizeof(m_sAudioContext));
}

SDL_AudioDevice::~SDL_AudioDevice()
{
    if (m_pMutePCM) ECMemFree(m_pMutePCM);
}

int SDL_AudioDevice::Init(MediaContext* pMediaCtx)
{
    m_sAudioContext.silence = 0;
    m_sAudioContext.userdata = this;
    m_sAudioContext.format = AUDIO_S16SYS;
    m_sAudioContext.freq = pMediaCtx->nSampleRate;
    m_sAudioContext.channels = RESAMPLE_OUT_CHANNEL;
    m_sAudioContext.callback = PlaySoundCallback;
    m_sAudioContext.samples = pMediaCtx->nSampleSize;

    if(SDL_Init(SDL_INIT_AUDIO) < 0)
        return Audio_Render_Err_DeviceErr;
    if(SDL_OpenAudio(&m_sAudioContext, NULL) < 0)
        return Audio_Render_Err_DeviceErr;

    ResampleParam param;
    param.input_channels = pMediaCtx->nChannels;
    param.input_samplerate = pMediaCtx->nSampleRate;
    param.input_sample_fmt = pMediaCtx->nSampleFormat;
    param.output_channels = pMediaCtx->nChannels;
    param.output_samplerate = pMediaCtx->nSampleRate;
    param.output_sample_fmt = AV_SAMPLE_FMT_S16;
    m_pResample = new ReSample(param);

    return Audio_Render_Err_None;
}

void SDL_AudioDevice::Uninit()
{
    SDL_CloseAudio();
}

void SDL_AudioDevice::Run()
{
    SDL_PauseAudio(0);
}

void SDL_AudioDevice::Pause()
{
    SDL_PauseAudio(1);
}

int SDL_AudioDevice::SampleFmtSwitch(AudioSampleFormat nFmtIn)
{
    switch (nFmtIn)
    {
        case AudioSampleFormat_U8:
        case AudioSampleFormat_U8P:    return AUDIO_U8;
        case AudioSampleFormat_S16:
        case AudioSampleFormat_S16P:   return AUDIO_S16SYS;
        default: return AUDIO_S16SYS;
    }
}

void SDL_AudioDevice::PlaySoundCallback(void* pUserData, Uint8* pStream, int nLen)
{
    SDL_AudioDevice *pSelf = (SDL_AudioDevice*)pUserData;

    if (pSelf->m_PCMBufStream.DataSize() >= nLen)
    {
        char* pcmBuf =(char*)ECMemAlloc(nLen);
        EC_U32 nPCMSize = pSelf->m_PCMBufStream.Read(pcmBuf, nLen);
        SDL_MixAudio(pStream, (Uint8*)pcmBuf, nPCMSize, SDL_MIX_MAXVOLUME);
        ECMemFree(pcmBuf);
        return;
    }

    char* pPCM = NULL;
    char* pRePCM = NULL;
    unsigned int nMaxTry = 0;
    unsigned int nOutSize = 0;
    unsigned int nReOutSize = 0;
    unsigned int nOutSamples = 0;
    do{
        nMaxTry++;
        pSelf->m_pAudioRender->GetPCMBuffer(&pPCM, &nOutSize, &nOutSamples, true);
        if (pPCM && (nOutSize > 0))
        {
            pSelf->m_pResample->DoReSample(pPCM, nOutSamples, &pRePCM, &nReOutSize);
        }
        if (pSelf->m_PCMBufStream.DataSize() + nReOutSize < nLen)
        {
            pSelf->m_PCMBufStream.Write(pRePCM, nReOutSize);
            pPCM = NULL;
            pRePCM = NULL;
            nOutSize = 0;
            nReOutSize = 0;
            nOutSamples = 0;
        }
        else
        {
            break;
        }
    } while (nMaxTry <= 10);

    if (pSelf->m_PCMBufStream.DataSize() == 0)
    {
        /* all PCM data saved in pRePCM */
        if (nReOutSize == nLen)
        {
            SDL_MixAudio(pStream, (Uint8*)pRePCM, nReOutSize, SDL_MIX_MAXVOLUME);
        }
        else if (nReOutSize > nLen)
        {
            EC_U32 nExtraSize = nReOutSize - nLen;
            SDL_MixAudio(pStream, (Uint8*)pRePCM, nLen, SDL_MIX_MAXVOLUME);
            pSelf->m_PCMBufStream.Write(pRePCM + nLen, nExtraSize);
        }
        else
        {
            SDL_MixAudio(pStream, (Uint8*)pSelf->m_pMutePCM, pSelf->m_nMuteBufSize, SDL_MIX_MAXVOLUME);
        }
    }
    else
    {
        /* Part/All PCM data saved in m_PCMBufStream,
           we will store all PCM data to m_PCMBufStream */
        pSelf->m_PCMBufStream.Write(pRePCM, nReOutSize);
        char* pPCMBuf = (char*)ECMemAlloc(nLen);
        EC_U32 nPCMSize = pSelf->m_PCMBufStream.Read(pPCMBuf, nLen);
        SDL_MixAudio(pStream, (Uint8*)pPCMBuf, nPCMSize, SDL_MIX_MAXVOLUME);
        ECMemFree(pPCMBuf);
    }
}
