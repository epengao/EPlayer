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
 * FrameScale.h
 * This file for all video frame scaling interface define.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef FRAME_SCALE_H
#define FRAME_SCALE_H

#include "ECType.h"

#ifdef __cplusplus
#define __STDC_CONSTANT_MACROS
#endif
extern "C"
{
#include "libavutil/imgutils.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavformat/avformat.h"
}

typedef struct {
    int inputWidth;
    int inputHeight;
    AVPixelFormat inputFrameFormat;
    int outputWidth;
    int outputHeight;
    AVPixelFormat outputFrameFormat;
} ScaleParam;

class FrameScale
{
public:
    FrameScale(ScaleParam param);
    ~FrameScale();

    void SetScaleParam(ScaleParam *inParam);
    void GetScaleParam(ScaleParam *outParam);
    void UpdateScaleOutput(EC_U32 nNewWidth, EC_U32 nNewHeight);
    void DoScalingFrame(AVFrame* pInputFrame, AVFrame** ppOutputFrame);
    void DoScaling(AVFrame* pInputFrame, EC_U8** ppOutFrameBuf, EC_U32* pOutBufSize);

private:
    bool CheckScaleParamDiff(ScaleParam* pParam);

private:
    bool               m_realScale;
    ScaleParam         m_ScaleParam;
    AVFrame*           m_pVideoFrame;
    unsigned char*     m_pVideoFrameBuf;
    struct SwsContext* m_pFrameScaleContext;
};

#endif  /* FRAME_SCALE_H */
