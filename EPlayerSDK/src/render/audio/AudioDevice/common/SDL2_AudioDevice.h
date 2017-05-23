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
 * SDL2_AudioDevice.h
 * This file for SDL2 audio sound output device
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef SDL2_AUDIO_DEVICE_H
#define SDL2_AUDIO_DEVICE_H

extern "C"
{
#include "SDL2/SDL.h"
};
#include "ReSample.h"
#include "AudioDeviceI.h"

#define RESAMPLE_OUT_RATE 44100
#define RESAMPLE_OUT_CHANNEL  2
#define MUTE_SOUND_BUF_SIZE 128

class SDL2_AudioDevice : public AudioDeviceI
{
public:
    SDL2_AudioDevice(AudioRender* pAudioRender);
    ~SDL2_AudioDevice();

    int Init(MediaContext* pMediaCtx);
    void Uninit();
    void Run();
    void Pause();
    void Flush() {};
    static void PlaySoundCallback(void* pUserData, Uint8 *pStream, int nLen);

private:
    int SampleFmtSwitch(AudioSampleFormat nFmtIn);

private:
    EC_U8*        m_pMutePCM;
    EC_U32        m_nMuteBufSize;
	ReSample*     m_pResample;
    SDL_AudioSpec m_sAudioContext;
};

#endif /* SDL2_AUDIO_DEVICE_H */
