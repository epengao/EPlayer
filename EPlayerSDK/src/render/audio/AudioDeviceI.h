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
 * AudioDevice.h
 * This file for AudioDevice class define.
 * This module for abstract of the OS platform for SoundOutput
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef AUDIO_DEVICE_I_H
#define AUDIO_DEVICE_I_H

#include "ECType.h"
#include "AudioRender.h"

class AudioRender;

class AudioDeviceI
{
public:
    AudioDeviceI(AudioRender* pAudioRender) { m_pAudioRender = pAudioRender; }
    virtual ~AudioDeviceI(){};
    virtual int Init(MediaContext* pMediaCtx) = 0;
    virtual void Uninit() = 0;
    virtual void Run() = 0;
    virtual void Pause() = 0;
    virtual void Flush() = 0;

protected:
    AudioRender*  m_pAudioRender;
};

#endif /* AUDIO_DEVICE_I_H */
