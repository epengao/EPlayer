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
 * SDL2_VideoDevice.h
 * Here we define a class to Package the SDL2 video output.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef SDL2_VIDEO_DEVICE_H
#define SDL2_VIDEO_DEVICE_H

#define VIDEO_POSI_ALIGN 2
#define VIDEO_SIZE_ALIGN 4

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

extern "C"
{
#include "SDL2/SDL.h"
};
#include "FrameScale.h"
#include "VideoDeviceI.h"

class SDL2_VideoDevice : public VideoDeviceI
{
public:
    SDL2_VideoDevice();
    ~SDL2_VideoDevice();
    int Init(void* pVideoWindow,
             int nWindowWidth,
             int nWindowHeight,
             MediaContext* pMediaContext);
    void Uninit();
    void Flush();
    void DrawFrame(VideoFrame *pFrame);
    void UpdateVideoWindow(void* pWindow, int nWidth, int nHeight);

private:
    void SetEqualScaling(SDL_Rect *pRect);
    void SetEqualScalingRotaR90(SDL_Rect *pRect);

private:
    SDL_Rect            m_FrameRect;
    SDL_Window*         m_pSDLWindow;
    SDL_Texture*        m_pSDLTexture;
    SDL_Renderer*       m_pSDLRenderer;
private:
    EC_U32              m_nVideoWidth;
    EC_U32              m_nVideoHeight;
    EC_U32              m_nVideoWndWidth;
    EC_U32              m_nVideoWndHeight;
    EC_PTR              m_pVideoWindow;
    FrameScale*         m_pVideoFrameScale;
    VideoRotation       m_nVideoRotation;
};

#endif /* SDL_VIDEO_DEVICE_H */
