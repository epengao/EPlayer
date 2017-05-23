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
 * SDL_VideoDevice.h
 * Here we define a class to Package the SDL video output.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef SDL_VIDEO_DEVICE_H
#define SDL_VIDEO_DEVICE_H

#define VIDEO_POSI_ALIGN 2
#define VIDEO_SIZE_ALIGN 4

extern "C"
{
#include "SDL/SDL.h"
};
#include "FrameScale.h"
#include "VideoDeviceI.h"

class SDL_VideoDevice : public VideoDeviceI
{
public:
    SDL_VideoDevice();
    ~SDL_VideoDevice();
    int Init(void* pVideoWindow,
             int nWindowWidth,
             int nWindowHeight,
             MediaContext* pMediaContext);
    void Uninit();
    void Flush();
    void DrawFrame(VideoFrame *pFrame);
    void UpdateVideoWindow(void* pWindow, int nWidth, int nHeight);

private:
    void EraseVideoRim();
    void SetEqualScaling(SDL_Rect *pRect);

private:
    EC_PTR       m_pUserWnd;
    SDL_Rect     m_sSDLRect;
    SDL_Surface* m_pSDLSurface;
    SDL_Overlay* m_pSDLOverlay;
    FrameScale*  m_pVideoFrameScale;
private:
    EC_U32       m_nVideoWidth;
    EC_U32       m_nVideoHeight;
    EC_U32       m_nUserWndWidth;
    EC_U32       m_nUserWndHeight;
    EC_U32       m_nDeviceScreenWidth;
    EC_U32       m_nDeviceScreenHeight;
};

#endif /* SDL_VIDEO_DEVICE_H */
