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
 * FrameScale.cpp
 * This file for all video scaling interface implement.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#include "ECLog.h"
#include "ECMemOP.h"
#include "FrameScale.h"

FrameScale::FrameScale(ScaleParam param)
:m_ScaleParam(param)
,m_pVideoFrame(NULL)
,m_pVideoFrameBuf(NULL)
,m_pFrameScaleContext(NULL)
{
    SetScaleParam(&m_ScaleParam);
}

FrameScale::~FrameScale()
{
    if(m_pVideoFrame)
    {
        av_frame_free(&m_pVideoFrame);
    }
    if(m_pVideoFrameBuf)
    {
        av_free(m_pVideoFrameBuf);
    }
    if(m_pFrameScaleContext)
    {
        sws_freeContext(m_pFrameScaleContext);
    }
}

void FrameScale::SetScaleParam(ScaleParam *inParam)
{
    m_realScale = true;
    /* m_realScale = CheckScaleParamDiff(inParam); */
    if(inParam && m_realScale)
    {
        ECMemCopy(&m_ScaleParam, inParam, sizeof(ScaleParam));
        if(m_pFrameScaleContext) sws_freeContext(m_pFrameScaleContext);
        m_pFrameScaleContext = sws_getContext(m_ScaleParam.inputWidth,
                                              m_ScaleParam.inputHeight,
                                              m_ScaleParam.inputFrameFormat,
                                              m_ScaleParam.outputWidth,
                                              m_ScaleParam.outputHeight,
                                              m_ScaleParam.outputFrameFormat,
                                              SWS_FAST_BILINEAR, NULL, NULL, NULL);
        if(m_pVideoFrame)
        {
            av_frame_free(&m_pVideoFrame);
        }
        if(m_pVideoFrameBuf)
        {
            av_free(m_pVideoFrameBuf);
        }
        m_pVideoFrame = av_frame_alloc();
        int bufSize = av_image_get_buffer_size(m_ScaleParam.outputFrameFormat,
                                               m_ScaleParam.outputWidth, m_ScaleParam.outputHeight, 1);
        m_pVideoFrameBuf = (unsigned char*)av_malloc(bufSize);
        av_image_fill_arrays(m_pVideoFrame->data, m_pVideoFrame->linesize, m_pVideoFrameBuf,
                             m_ScaleParam.outputFrameFormat, m_ScaleParam.outputWidth, m_ScaleParam.outputHeight, 1);
    }
}

void FrameScale::GetScaleParam(ScaleParam *outParam)
{
    if(outParam)
    {
        ECMemCopy(outParam, &m_ScaleParam, sizeof(ScaleParam));
    }
}

void FrameScale::UpdateScaleOutput(EC_U32 nNewWidth, EC_U32 nNewHeight)
{
    m_ScaleParam.outputWidth = nNewWidth;
    m_ScaleParam.outputHeight = nNewHeight;
    SetScaleParam(&m_ScaleParam);
}

void FrameScale::DoScalingFrame(AVFrame* pInputFrame, AVFrame** ppOutputFrame)
{
    if(pInputFrame && ppOutputFrame)
    {
        if(m_realScale)
        {
            sws_scale(m_pFrameScaleContext, pInputFrame->data,
                pInputFrame->linesize, 0, m_ScaleParam.inputHeight, m_pVideoFrame->data, m_pVideoFrame->linesize);
            (*ppOutputFrame) = m_pVideoFrame;
        }
        else
        {
            (*ppOutputFrame) = pInputFrame;
        }
    }
}

void FrameScale::DoScaling(AVFrame* pInputFrame, EC_U8** ppOutFrameBuf, EC_U32* pOutBufSize)
{
    if(pInputFrame)
    {
        if(m_realScale && ppOutFrameBuf && pOutBufSize)
        {
            sws_scale(m_pFrameScaleContext, pInputFrame->data,
                      pInputFrame->linesize, 0, m_ScaleParam.inputHeight, m_pVideoFrame->data, m_pVideoFrame->linesize);
            *ppOutFrameBuf = m_pVideoFrame->data[0];
            *pOutBufSize = m_pVideoFrame->linesize[0];
        }
        else
        {
            *ppOutFrameBuf = pInputFrame->data[0];
            *pOutBufSize = pInputFrame->linesize[0];
        }
    }
}

/* Private Method */
bool FrameScale::CheckScaleParamDiff(ScaleParam* pParam)
{
    return (pParam->inputWidth != pParam->outputWidth) ||
           (pParam->inputHeight != pParam->outputHeight) ||
           (pParam->inputFrameFormat != pParam->outputFrameFormat);
}
