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
 * iOS_VideoDevice.h
 * Here we define a class to Package the iOS platform video output.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef IOS_VIDEO_DEVICE_H
#define IOS_VIDEO_DEVICE_H

@class VideoWindow;
#include "VideoDeviceI.h"

class iOS_VideoDevice : public VideoDeviceI
{
public:
    iOS_VideoDevice();
    ~iOS_VideoDevice();
    int Init(void* pVideoWindow,
             int nWindowWidth,
             int nWindowHeight,
             MediaContext* pMediaContext);
    void Uninit();
    void Flush();
    void DrawFrame(VideoFrame *pFrame);
    void UpdateVideoWindow(void* pWindow, int nWidth, int nHeight);
private:
    void ClipVideoDrawRect();
private:
    VideoWindow* m_pUserWnd;
    EC_U32       m_nVideoWidth;
    EC_U32       m_nVideoHeight;
    EC_U32       m_nUserWndWidth;
    EC_U32       m_nUserWndHeight;
};

#endif /* IOS_VIDEO_DEVICE_H */
