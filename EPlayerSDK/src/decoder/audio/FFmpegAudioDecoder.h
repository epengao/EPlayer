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
 * FFmpegAudioDecoder.h
 * This file for audio FFMPEG decoder interface define.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef FFMPEG_AUDIO_DECODER_H
#define FFMPEG_AUDIO_DECODER_H

#include "ECType.h"
#include "AudioDecoder.h"
#include "MediaSource.h"

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
};

class FFmpegAudioDecoder
{
public:
    FFmpegAudioDecoder();
    ~FFmpegAudioDecoder();
    EC_U32 OpenDecoder(MediaContext* pMediaContext);
    void CloseDecoder();
    void FlushDecoder();
    EC_U32 SetInputPacket(SourcePacket* pInputPacket);
    EC_U32 GetOutputFrame(AudioFrame* pOutputAudioFrame);

private:
    AVFrame*           m_pPCM;
    bool               m_PlanarPCM;
    AVCodecContext*    m_pCodecCtx;
    AVCodecParameters* m_pCodecParam;
    double             m_TimeBase;
    AudioFrameBuffer   m_AudioFrameBuf;
    EC_U32             m_nMaxPCMBufSize;
};

#endif /* FFMPEG_AUDIO_DECODER_H */
