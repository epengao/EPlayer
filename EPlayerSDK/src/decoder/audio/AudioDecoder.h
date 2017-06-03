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
 * AudioDecoder.h
 * This file for Media audio decoder module class define.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef AUDIO_DECODER_H
#define AUDIO_DECODER_H

#include "ECMutex.h"
#include "ECThreadDriver.h"
#include "SourceAudioPort.h"

#define Audio_Dec_Err_None            0x00000000
#define Audio_Dec_Err_ReadEOS         0x000F0001
#define Audio_Dec_Err_OpenFaild       0x000F0002
#define Audio_Dec_Err_SkipPkt         0x000F0003
#define Audio_Dec_Err_KeepPkt         0x000F0004
#define Audio_Dec_Err_NeedNewPkt      0x000F0005
#define Audio_Dec_Err_PCMBufSmall     0x000F0006
#define Audio_Dec_Err_ReadPCMBufFaild 0x000F0007

#define AUDIO_DEC_WAIT_TIME  10

typedef struct
{
    char* pPCMBuf;
    EC_U32 nSamples;
    EC_U32 nBufSize;
    EC_U32 nDataSize;
    EC_U64 nTimestamp;
} AudioFrameBuffer;

typedef struct
{
    char* pPCMData;
    EC_U32 nDataSize;
    EC_U32 nSamples;
    EC_U64 nTimestamp;
} AudioFrame;

class AudioDecPort;
class FFmpegAudioDecoder;

class AudioDecoder
{
public:
    AudioDecoder();
    ~AudioDecoder();

    EC_U32 OpenDecoder(MediaContext* pMediaContext,
                       SourceAudioPort* pSourcePort);
    void CloseDecoder();
    void Flush();
    EC_U32 GetAudioFrame(AudioFrame* pFrameOut, bool rawFFPCMBuf = false);
    AudioDecPort* GetAudioDecPort() { return m_pPort; };

private:
    ECMutex             m_mtxDec;
    SourcePacket*       m_pCurrPacket;
    AudioDecPort*       m_pPort;
    SourceAudioPort*    m_pSourcePort;
    FFmpegAudioDecoder* m_pFFmpegDecoder;
};

#endif /* AUDIO_DECODER_H */
