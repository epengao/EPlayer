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
 * Win_VideoDevice.h
 * Here we define a class to Package the Windows platform video output.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef WIN_VIDEO_DEVICE_H
#define WIN_VIDEO_DEVICE_H

#include "VideoDeviceI.h"

class Win_VideoDevice : public VideoDeviceI
{
public:
    Win_VideoDevice();
    ~Win_VideoDevice();
    int Init(void* pVideoWindow,
             int nWindowWidth,
             int nWindowHeight,
             MediaContext* pMediaContext);
    void Uninit();
    void Flush();
    void DrawFrame(VideoFrame *pFrame);
    void UpdateVideoWindow(void* pWindow, int nWidth, int nHeight);

private:
    VideoDeviceI *m_pRndDev;
};

#endif /* WIN_VIDEO_DEVICE_H */
