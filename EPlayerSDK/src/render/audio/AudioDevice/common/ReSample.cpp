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
 * ReSample.cpp
 * This file for all audio PCM resample interface implement.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#include "ReSample.h"

ReSample::ReSample(ResampleParam param)
:m_nResampBufSize(0)
,m_ResampleOutbuf(NULL)
,m_PcmResampleContext(NULL)
{
    m_AudioFilterParam.input_channels = param.input_channels;
    m_AudioFilterParam.input_samplerate = param.input_samplerate;
    m_AudioFilterParam.input_sample_fmt = param.input_sample_fmt;
    m_AudioFilterParam.output_channels = param.output_channels;
    m_AudioFilterParam.output_samplerate = param.output_samplerate;
    m_AudioFilterParam.output_sample_fmt = param.output_sample_fmt;
    m_nInChannelsLayout = (int)av_get_default_channel_layout(param.input_channels);
    m_nOutChannelsLayout = (int)av_get_default_channel_layout(param.output_channels);

    m_PcmResampleContext = swr_alloc();
    m_PcmResampleContext = swr_alloc_set_opts(m_PcmResampleContext,
                                              m_nOutChannelsLayout,
                                              (AVSampleFormat)param.output_sample_fmt,
                                              m_AudioFilterParam.output_samplerate,
                                              m_nInChannelsLayout,
                                              (AVSampleFormat)param.input_sample_fmt,
                                              m_AudioFilterParam.input_samplerate, 0, NULL);
    swr_init(m_PcmResampleContext);
    m_nResampBufSize = MAX_PCM_OUT_BUF_SIZE * param.output_channels * 4;
    m_ResampleOutbuf = (uint8_t *)av_malloc(m_nResampBufSize);
}

ReSample::~ReSample()
{
    av_free(m_ResampleOutbuf);
    swr_free(&m_PcmResampleContext);
}

void ReSample::SetFilterParam(ResampleParam inParam)
{
    if ((m_AudioFilterParam.input_channels != inParam.input_channels) ||
        (m_AudioFilterParam.input_samplerate != inParam.input_samplerate) ||
        (m_AudioFilterParam.input_sample_fmt != inParam.input_sample_fmt) )
    {
        m_AudioFilterParam.input_channels = inParam.input_channels;
        m_AudioFilterParam.input_samplerate = inParam.input_samplerate;
        m_AudioFilterParam.input_sample_fmt = inParam.input_sample_fmt;
        m_AudioFilterParam.output_channels = inParam.output_channels;
        m_AudioFilterParam.output_samplerate = inParam.output_samplerate;
        m_AudioFilterParam.output_sample_fmt = inParam.output_sample_fmt;
        m_nInChannelsLayout = (int)av_get_default_channel_layout(inParam.input_channels);
        m_nOutChannelsLayout = (int)av_get_default_channel_layout(inParam.output_channels);

        if (m_PcmResampleContext == NULL)
        {
            m_PcmResampleContext = swr_alloc();
        }
        m_PcmResampleContext = swr_alloc_set_opts(m_PcmResampleContext,
                                                  m_nOutChannelsLayout,
                                                  (AVSampleFormat)inParam.output_sample_fmt,
                                                  m_AudioFilterParam.output_samplerate,
                                                  m_nInChannelsLayout,
                                                  (AVSampleFormat)inParam.input_sample_fmt,
                                                  m_AudioFilterParam.input_samplerate, 0, NULL);
        swr_init(m_PcmResampleContext);
        if (m_ResampleOutbuf == NULL )
        {
            m_ResampleOutbuf = (uint8_t *)av_malloc(m_nResampBufSize);
        }
    }
}

void ReSample::GetFilterParam(ResampleParam *outFilterParam)
{
    if (outFilterParam)
    {
        outFilterParam->input_channels = m_AudioFilterParam.input_channels;
        outFilterParam->input_samplerate = m_AudioFilterParam.input_samplerate ;
        outFilterParam->input_sample_fmt = m_AudioFilterParam.input_sample_fmt;

        outFilterParam->output_channels = m_AudioFilterParam.output_channels;
        outFilterParam->output_samplerate = m_AudioFilterParam.output_samplerate;
        outFilterParam->output_sample_fmt = m_AudioFilterParam.input_sample_fmt;
    }
}

void ReSample::DoReSample(char* pPcmIn, EC_U32 nInSamples, char **ppPcmOut, EC_U32* pOutSize)
{
    swr_convert(m_PcmResampleContext,
                &m_ResampleOutbuf, m_nResampBufSize,
                (const uint8_t **)pPcmIn, nInSamples);
    int nOutBufSize = av_samples_get_buffer_size(NULL, m_AudioFilterParam.output_channels, nInSamples,
                                                (AVSampleFormat)m_AudioFilterParam.output_sample_fmt, 1);
    (*pOutSize) = nOutBufSize;
    (*ppPcmOut) = (char*)m_ResampleOutbuf;
}
