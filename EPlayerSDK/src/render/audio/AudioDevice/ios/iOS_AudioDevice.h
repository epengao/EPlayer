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
 * iOS_AudioDevice.h
 * Here we define a class to Package the iOS platform audio output.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef IOS_AUDIO_DEVICE_H
#define IOS_AUDIO_DEVICE_H

#define MAX_BUFFER_SIZE     352800
#define MUTE_SOUND_SIZE        128
#define QUEUE_BUFFER_COUNT       2
#define WAIT_BUF_SOUND_SIZE     64

#include "AudioDeviceI.h"
#include <AudioToolbox/AudioToolbox.h>

class iOS_AudioDevice : public AudioDeviceI
{
public:
    iOS_AudioDevice(AudioRender* pAudioRender);
    ~iOS_AudioDevice(){};

    int Init(MediaContext* pMediaCtx);
    void Uninit();
    void Run();
    void Pause();
    void Flush();

private:
    UInt32 SmpleBitSwitch(AudioSampleFormat nFmtIn);
    UInt32 SampleFmtSwitch(AudioSampleFormat nFmtIn);
    static void PlaySoundCallback(void* pUserData,
                                  AudioQueueRef inQueue,
                                  AudioQueueBufferRef outQueueBuf);

private:
    bool                        m_Running;
    AudioQueueRef               m_rAudioQueue;
    AudioStreamBasicDescription m_sAudioDescription;
    AudioQueueBufferRef         m_sCurrAudioQueueBuf;
    AudioQueueBufferRef         m_sAudioQueueBuffers[QUEUE_BUFFER_COUNT];
};

#endif /* IOS_AUDIO_DEVICE_H */
