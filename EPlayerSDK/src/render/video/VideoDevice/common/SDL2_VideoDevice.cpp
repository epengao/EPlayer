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
 * SDL2_VideoDevice.cpp
 * This file is SDL2_VideoDevice class implementation
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#include "ECLog.h"
#include "ECUtil.h"
#include "ECMemOP.h"
#include "SDL2_VideoDevice.h"


SDL2_VideoDevice::SDL2_VideoDevice()
:m_nVideoWidth(0)
,m_nVideoHeight(0)
,m_nVideoWndWidth(0)
,m_nVideoWndHeight(0)
,m_pVideoWindow(NULL)
,m_pVideoFrameScale(NULL)
,m_nVideoRotation(VideoRotation_None)
{
}

SDL2_VideoDevice::~SDL2_VideoDevice()
{
}

int SDL2_VideoDevice::Init(void* pVideoWindow,
                           int nWindowWidth,
                           int nWindowHeight,
                           MediaContext* pMediaContext)
{
    m_pVideoWindow = pVideoWindow;
    m_nVideoWndWidth = nWindowWidth;
    m_nVideoWndHeight = nWindowHeight;
    m_nVideoWidth = pMediaContext->nVideoWidth;
    m_nVideoHeight = pMediaContext->nVideoHeight;
    m_nVideoRotation = VideoRotation_None;

    if(pMediaContext->videoRotation == -90)
    {
        m_nVideoRotation = VideoRotation_Right_90;
    }
    SetVideoRect();
    ScaleParam frameScaleParam;
    frameScaleParam.outputWidth = m_FrameRect.w;
    frameScaleParam.outputHeight = m_FrameRect.h;
    frameScaleParam.inputWidth = pMediaContext->nVideoWidth;
    frameScaleParam.inputHeight = pMediaContext->nVideoHeight;
    frameScaleParam.outputFrameFormat = AV_PIX_FMT_YUV420P;
    frameScaleParam.inputFrameFormat = (AVPixelFormat)pMediaContext->pVideoDecParam->format;
    m_pVideoFrameScale = new FrameScale(frameScaleParam);

    if(SDL_Init(SDL_INIT_VIDEO))
    {
        return Video_Render_Err_DeviceErr;
    }
    m_pSDLWindow = SDL_CreateWindowFrom(pVideoWindow);
    m_pSDLRenderer = SDL_CreateRenderer(m_pSDLWindow, -1, 0);
    m_pSDLTexture = SDL_CreateTexture(m_pSDLRenderer,
                                      SDL_PIXELFORMAT_IYUV,
                                      SDL_TEXTUREACCESS_STREAMING,
                                      m_nVideoWidth*2, m_nVideoHeight*2);
    return Video_Render_Err_None;
}

void SDL2_VideoDevice::Uninit()
{
    m_nVideoWidth = 0;
    m_nVideoHeight = 0;
    m_nVideoWndWidth = 0;
    m_nVideoWndHeight = 0;
    m_pVideoWindow = NULL;

    if(m_pVideoFrameScale)
    {
        delete m_pVideoFrameScale;
        m_pVideoFrameScale = NULL;
    }
    ECMemSet(&m_FrameRect, 0, sizeof(SDL_Rect));
    ECMemSet(&m_RenderRect, 0, sizeof(SDL_Rect));

    /* clear video screen to black */
    SDL_SetRenderDrawColor(m_pSDLRenderer, 0, 0, 0, 255);
    SDL_RenderClear(m_pSDLRenderer);
    SDL_RenderPresent(m_pSDLRenderer);
    /* release all the SDL resource */
    SDL_DestroyRenderer(m_pSDLRenderer);
    SDL_DestroyTexture(m_pSDLTexture);
    SDL_DestroyWindow(m_pSDLWindow);
    /* SDL_Quit(); can not Quit? Bug?*/
}

void SDL2_VideoDevice::Flush()
{
}

void SDL2_VideoDevice::DrawFrame(VideoFrame *pFrame)
{
    EC_U32 nBufSize = 0;
    EC_U8* pOutFrameBuf = NULL;
    m_pVideoFrameScale->DoScaling(pFrame->pAVFrame, &pOutFrameBuf, &nBufSize);

    if(pOutFrameBuf && nBufSize > 0)
    {
        SDL_UpdateTexture(m_pSDLTexture, &m_FrameRect, pOutFrameBuf, nBufSize);
        if(m_nVideoRotation == VideoRotation_Right_90)
        {
            static SDL_Point rotaPoint = {.x = 0, .y = 0};
            SDL_RenderCopyEx(m_pSDLRenderer, m_pSDLTexture, &m_FrameRect, &m_RenderRect, 90, &rotaPoint, SDL_FLIP_NONE);
        }
        else
        {
            SDL_RenderCopy(m_pSDLRenderer, m_pSDLTexture, &m_FrameRect, &m_FrameRect);
        }
        SDL_RenderPresent(m_pSDLRenderer);
    }
}

void SDL2_VideoDevice::UpdateVideoWindow(void* pWindow, int nWidth, int nHeight)
{
    bool updateWindow = (pWindow != m_pVideoWindow);
    bool updateVideoSize =((m_nVideoWndWidth != nWidth) ||
                           (m_nVideoWndHeight!= nHeight));

    if(updateWindow || updateVideoSize)
    {
        m_nVideoWndWidth = nWidth;
        m_nVideoWndHeight = nHeight;

        SetVideoRect();
        m_pVideoFrameScale->UpdateScaleOutput(m_FrameRect.w, m_FrameRect.h);

        if(updateWindow)
        {
            SDL_DestroyWindow(m_pSDLWindow);
            SDL_DestroyRenderer(m_pSDLRenderer);
            m_pSDLWindow = SDL_CreateWindowFrom(pWindow);
            m_pSDLRenderer = SDL_CreateRenderer(m_pSDLWindow, -1, 0);
        }
        SDL_DestroyTexture(m_pSDLTexture);
        m_pSDLTexture = SDL_CreateTexture(m_pSDLRenderer,
                                          SDL_PIXELFORMAT_IYUV,
                                          SDL_TEXTUREACCESS_STREAMING,
                                          m_nVideoWndWidth, m_nVideoWndHeight);
    }
}

/* Private Method */
void SDL2_VideoDevice::SetVideoRect()
{
    /* First we have set the 
     * render rect by the window */
    SetRenderRect();
    /* From the render rect we get
     ^ the video frame scale rect */
    SetFrameRect();
}

void SDL2_VideoDevice::SetFrameRect()
{
    if(m_nVideoRotation == VideoRotation_None)
    {
        m_FrameRect.x = m_RenderRect.x;
        m_FrameRect.y = m_RenderRect.y;
        m_FrameRect.w = m_RenderRect.w;
        m_FrameRect.h = m_RenderRect.h;
    }
    else if(m_nVideoRotation == VideoRotation_Right_90)
    {
        m_FrameRect.x = 0;
        m_FrameRect.y = 0;
        m_FrameRect.w = m_RenderRect.w;
        m_FrameRect.h = m_RenderRect.h;
        /* 
         * 1. rotation from point:{x=0,y=m_FrameRect.h}
         * 2. scale to new rect point:{x=-m_FrameRect.h,y=0}
         * 3. move new frame to m_RenderRect
         */
        int afterRotaY = 0;
        int afterRotaX = -1 * m_RenderRect.h;
        m_RenderRect.x = m_RenderRect.x - afterRotaX;
        m_RenderRect.y = m_RenderRect.y - afterRotaY;
    }
    else {/* TODO */}
}

void SDL2_VideoDevice::SetRenderRect()
{
    if(m_nVideoRotation == VideoRotation_None)
    {
        SetRenderRectNormal();
    }
    else if(m_nVideoRotation == VideoRotation_Right_90)
    {
        SetRenderRectRotaR90();
    }
    else {/* TODO */}
}

void SDL2_VideoDevice::SetRenderRectNormal()
{
    float ratio = 1.0f * (float)m_nVideoWidth / (float)m_nVideoHeight;
    EC_U32 nVideoScaleToHeight = m_nVideoWndHeight;
    EC_U32 nVideoScaleToWidth = (EC_U32)(ratio * nVideoScaleToHeight);

    if (nVideoScaleToWidth > m_nVideoWndWidth)
    {
        nVideoScaleToWidth = m_nVideoWndWidth;
        nVideoScaleToHeight = (EC_U32)((float)nVideoScaleToWidth / ratio);
    }

    nVideoScaleToWidth = (nVideoScaleToWidth >> 4) << 4;
    nVideoScaleToHeight = (nVideoScaleToHeight >> 4) << 4;
    m_RenderRect.w = nVideoScaleToWidth;
    m_RenderRect.h = nVideoScaleToHeight;
    m_RenderRect.x = (m_nVideoWndWidth - nVideoScaleToWidth) * 0.5;
    m_RenderRect.y = (m_nVideoWndHeight - nVideoScaleToHeight) * 0.5;

    EC_U32 xa = m_RenderRect.x % VIDEO_POSI_ALIGN;
    EC_U32 ya = m_RenderRect.y % VIDEO_POSI_ALIGN;
    m_RenderRect.x = m_RenderRect.x + xa;
    m_RenderRect.y = m_RenderRect.y + ya;
}

void SDL2_VideoDevice::SetRenderRectRotaR90()
{
    float ratio = 1.0f * (float)m_nVideoWidth / (float)m_nVideoHeight;
    EC_U32 nVideoScaleToWidth = m_nVideoWndHeight;
    EC_U32 nVideoScaleToHeight = (EC_U32)((float)nVideoScaleToWidth / ratio);

    if (nVideoScaleToHeight > m_nVideoWndWidth)
    {
        nVideoScaleToHeight = m_nVideoWndWidth;
        nVideoScaleToWidth = (EC_U32)((float)nVideoScaleToHeight * ratio);
    }

    nVideoScaleToWidth = (nVideoScaleToWidth >> 4) << 4;
    nVideoScaleToHeight = (nVideoScaleToHeight >> 4) << 4;
    m_RenderRect.w = nVideoScaleToWidth;
    m_RenderRect.h = nVideoScaleToHeight;
    m_RenderRect.x = (m_nVideoWndWidth - nVideoScaleToHeight) * 0.5;
    m_RenderRect.y = (m_nVideoWndHeight - nVideoScaleToWidth) * 0.5;

    EC_U32 xa = m_RenderRect.x % VIDEO_POSI_ALIGN;
    EC_U32 ya = m_RenderRect.y % VIDEO_POSI_ALIGN;
    m_RenderRect.x = m_RenderRect.x + xa;
    m_RenderRect.y = m_RenderRect.y + ya;
}
