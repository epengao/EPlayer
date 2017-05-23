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
 * AudioRender.h
 * This file for Audio decoder PCM data output to sound device.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef AUDIO_RENDER_H
#define AUDIO_RENDER_H

#include "ECType.h"
#include "ECMutex.h"
#include "MediaClock.h"
#include "AudioDecoder.h"

typedef enum
{
    AudioSampleFormat_U8,
    AudioSampleFormat_S16,
    AudioSampleFormat_S32,
    AudioSampleFormat_Float,
    AudioSampleFormat_Double,
    AudioSampleFormat_U8P,
    AudioSampleFormat_S16P,
    AudioSampleFormat_S32P,
    AudioSampleFormat_FloatP,
    AudioSampleFormat_DoubleP,
    AudioSampleFormat_S64,
    AudioSampleFormat_S64P,
    AudioSampleFormat_NB,
    AudioSampleFormat_Error = -1,
} AudioSampleFormat;

#define Audio_Render_Err_None            0x00000000
#define Audio_Render_Err_DeviceErr       0x00F00001

class AudioDeviceI;

class AudioRender
{
public:
    AudioRender(MediaClock* pClock = NULL);
    ~AudioRender();

    void Run();
    void Pause();
    void Flush();
    void Seek(EC_U32 nPos);
    EC_U32 OpenDevice(MediaContext* pMediaContext,
                      AudioDecPort* pAudioDecPort);
    void CloseDevice();
    void GetPCMBuffer(char** ppOutBuf, EC_U32* pOutSize, EC_U32* pOutSamples, bool rawData = false);
    AudioSampleFormat SampleFmtSwitch(int nFmtIn);

private:
    bool          m_bEOS;
	bool          m_bRawPCM;
    bool          m_bBufing;
    bool          m_bRunning;
    EC_U32        m_nLastRndTime;
    MediaClock*   m_pMedaiClock;
    AudioDeviceI* m_pAudioDevice;
    AudioDecPort* m_pDecoderPort;
};

#endif /* AUDIO_RENDER_H */
