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
 * ReSample.h
 * This file for all audio PCM resample interface define.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef RESAMPLE_H
#define RESAMPLE_H

#include "ECType.h"
extern "C"
{
#include "libavutil/opt.h"
#include "libavutil/avutil.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
}

#define  OUT_PCM_SAMPLE_RATE    44100
#define  MAX_PCM_OUT_BUF_SIZE  192000

typedef struct {
    int input_channels;
    int input_samplerate;
    int input_sample_fmt;
    int output_channels;
    int output_samplerate;
    int output_sample_fmt;
} ResampleParam;

class ReSample
{
public:
    ReSample(ResampleParam param);
    ~ReSample();

    void SetFilterParam(ResampleParam inParam);
    void GetFilterParam(ResampleParam *outParam);
    void DoReSample(char* pPcmIn, EC_U32 nInSamples, char **ppPcmOut, EC_U32* pOutSize);

private:
    int                m_nResampBufSize;
    uint8_t*           m_ResampleOutbuf;
    ResampleParam      m_AudioFilterParam;
    int                m_nInChannelsLayout;
    int                m_nOutChannelsLayout;
    struct SwrContext* m_PcmResampleContext;
};

#endif  /* RESAMPLE_H */
