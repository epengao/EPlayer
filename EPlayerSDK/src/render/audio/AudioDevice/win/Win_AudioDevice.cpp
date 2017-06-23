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
 * Win_AudioDevice.cpp
 * This file for Win32 platform audio sound output device
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#include "Win_AudioDevice.h"
#include "SDL_AudioDevice.h"

Win_AudioDevice::Win_AudioDevice(AudioRender* pAudioRender)
:AudioDeviceI(pAudioRender)
{
    m_AudioDevice = new SDL_AudioDevice(pAudioRender);
}

Win_AudioDevice::~Win_AudioDevice()
{
    delete m_AudioDevice;
}

int Win_AudioDevice::Init(MediaContext* pMediaCtx)
{
    return m_AudioDevice->Init(pMediaCtx);
}

void Win_AudioDevice::Uninit()
{
    m_AudioDevice->Uninit();
}

void Win_AudioDevice::Run()
{
    m_AudioDevice->Run();
}

void Win_AudioDevice::Pause()
{
    m_AudioDevice->Pause();
}
