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
 * Win_AudioDevice.h
 * This file for Win32 platform audio sound output device
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef WIN_AUDIO_DEVICE_H
#define WIN_AUDIO_DEVICE_H

#include "AudioRender.h"
#include "AudioDeviceI.h"
#include "SDL_AudioDevice.h"

class Win_AudioDevice : public AudioDeviceI
{
public:
    Win_AudioDevice(AudioRender* pAudioRender);
    ~Win_AudioDevice();

    int Init(MediaContext* pMediaCtx);
    void Uninit();
    void Run();
    void Pause();
    void Flush(){};

private:
    SDL_AudioDevice* m_AudioDevice;
};

#endif /* WIN_AUDIO_DEVICE_H */
