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
 * Mac_VideoDevice.h
 * Here we define a class to Package the MacOS platform video output.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef MAC_VIDEO_DEVICE_H
#define MAC_VIDEO_DEVICE_H

#include "VideoDeviceI.h"
#include "SDL2_VideoDevice.h"

class Mac_VideoDevice : public VideoDeviceI
{
public:
    Mac_VideoDevice();
    ~Mac_VideoDevice();
    int Init(void* pVideoWindow,
             int nWindowWidth,
             int nWindowHeight,
             MediaContext* pMediaContext);
    void Uninit();
    void Flush();
    void DrawFrame(VideoFrame *pFrame);
    void UpdateVideoWindow(void* pWindow, int nWidth, int nHeight);

private:
    SDL2_VideoDevice *m_pRndDev;
};

#endif /* MAC_VIDEO_DEVICE_H */
