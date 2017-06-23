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
 * VideoDevice.h
 * This file for VideoDevice class define.
 * This module for abstract of the OS platform for SoundOutput
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef VIDEO_DEVICE_I_H
#define VIDEO_DEVICE_I_H

#include "ECType.h"
#include "VideoDecoder.h"

typedef enum
{
    VideoRotation_None      = 0,
    VideoRotation_Left_90   = 1,
    VideoRotation_Left_180  = 2,
    VideoRotation_Left_270  = 3,
    VideoRotation_Right_90  = 4,
    VideoRotation_Right_180 = 5,
    VideoRotation_Right_270 = 6,
} VideoRotation;

#define Video_Render_Err_None            0x00000000
#define Video_Render_Err_DeviceErr       0x0F000001

class VideoDeviceI
{
public:
    virtual ~VideoDeviceI(){};
    virtual int Init(void* pVideoWindow,
                     int nWindowWidth,
                     int nWindowHeight,
                     MediaContext* pMediaContext) = 0;
    virtual void Uninit() = 0;
    virtual void Flush() = 0;
    virtual void DrawFrame(VideoFrame *pFrame) = 0;
    virtual void UpdateVideoWindow(void* pWindow, int nWidth, int nHeight) = 0;
};

#endif /* VIDEO_DEVICE_I_H */
