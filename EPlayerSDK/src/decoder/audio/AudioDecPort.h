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
 * AudioDecPort.h
 * This file define a Port, for other moduels get PCM data.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef AUDIO_DEC_PORT_H
#define AUDIO_DEC_PORT_H

#include "ECType.h"
#include "AudioDecoder.h"

class AudioDecPort
{
public:
    AudioDecPort(AudioDecoder *pDecoder);
    ~AudioDecPort();
    EC_U32 GetAudioFrame(AudioFrame* pFrameOut);

private:
    AudioDecoder*    m_pOwner;
};

#endif /* AUDIO_DEC_PORT_H */
