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
 * SDL_VideoDevice.cpp
 * This file is SDL_VideoDevice class implementation
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#include "ECLog.h"
#include "ECUtil.h"
#include "ECMemOP.h"
#include <Windows.h>
#include "SDL_VideoDevice.h"


SDL_VideoDevice::SDL_VideoDevice()
:m_pUserWnd(NULL)
,m_pSDLSurface(NULL)
,m_pSDLOverlay(NULL)
,m_nVideoWidth(0)
,m_nVideoHeight(0)
,m_nUserWndWidth(0)
,m_nUserWndHeight(0)
,m_nDeviceScreenWidth(0)
,m_nDeviceScreenHeight(0)
,m_pVideoFrameScale(NULL)
{
}

SDL_VideoDevice::~SDL_VideoDevice()
{
}

int SDL_VideoDevice::Init(void* pVideoWindow,
                          int nWindowWidth,
                          int nWindowHeight,
                          MediaContext* pMediaContext)
{
    m_pUserWnd = pVideoWindow;
    m_nUserWndWidth = nWindowWidth;
    m_nUserWndHeight = nWindowHeight;
    m_nVideoWidth = pMediaContext->nVideoWidth;
    m_nVideoHeight = pMediaContext->nVideoHeight;

    ScaleParam frameScaleParam;
    SetEqualScaling(&m_sSDLRect);
    frameScaleParam.outputWidth = m_sSDLRect.w;
    frameScaleParam.outputHeight = m_sSDLRect.h;
    frameScaleParam.inputWidth = pMediaContext->nVideoWidth;
    frameScaleParam.inputHeight = pMediaContext->nVideoHeight;
    frameScaleParam.outputFrameFormat = AV_PIX_FMT_YUV420P;
    frameScaleParam.inputFrameFormat = (AVPixelFormat)pMediaContext->pVideoDecParam->format;
    m_pVideoFrameScale = new FrameScale(frameScaleParam);

    char pEnvStr[256] = { 0 };
    sprintf_s(pEnvStr, 255, "SDL_WINDOWID=0x%lx", m_pUserWnd);
    SDL_putenv(pEnvStr);

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return Video_Render_Err_DeviceErr;

    m_nDeviceScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    m_nDeviceScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    EC_U32 nFlag = 0;
    if (m_pUserWnd == NULL)
        nFlag = SDL_FULLSCREEN;
    m_pSDLSurface = SDL_SetVideoMode(m_nDeviceScreenWidth, m_nDeviceScreenHeight, 0, nFlag);
    if (m_pSDLSurface == NULL)
        return Video_Render_Err_DeviceErr;
    m_pSDLOverlay = SDL_CreateYUVOverlay(m_nUserWndWidth, m_nUserWndHeight, SDL_IYUV_OVERLAY, m_pSDLSurface);
    if (m_pSDLOverlay == NULL)
        return Video_Render_Err_DeviceErr;

    return Video_Render_Err_None;
}

void SDL_VideoDevice::Uninit()
{
    m_pUserWnd = NULL;
    m_nVideoWidth = 0;
    m_nVideoHeight = 0;
    m_nUserWndWidth = 0;
    m_nUserWndHeight = 0;
    m_nDeviceScreenWidth = 0;
    m_nDeviceScreenHeight = 0;

    if(m_pVideoFrameScale)
    {
        delete m_pVideoFrameScale;
        m_pVideoFrameScale = NULL;
    }
    ECMemSet(&m_sSDLRect, 0, sizeof(SDL_Rect));

    SDL_FreeSurface(m_pSDLSurface);
    SDL_FreeYUVOverlay(m_pSDLOverlay);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();
}

void SDL_VideoDevice::Flush()
{
}

void SDL_VideoDevice::DrawFrame(VideoFrame *pFrame)
{
    if (pFrame)
    {
        AVFrame* pOutFrame = NULL;
        m_pVideoFrameScale->DoScalingFrame(pFrame->pAVFrame, &pOutFrame);

        if (pOutFrame != NULL)
        {
            m_pSDLOverlay->pixels[0] = pOutFrame->data[0];
            m_pSDLOverlay->pixels[1] = pOutFrame->data[1];
            m_pSDLOverlay->pixels[2] = pOutFrame->data[2];
            m_pSDLOverlay->pitches[0] = pOutFrame->linesize[0];
            m_pSDLOverlay->pitches[1] = pOutFrame->linesize[1];
            m_pSDLOverlay->pitches[2] = pOutFrame->linesize[2];
            SDL_DisplayYUVOverlay(m_pSDLOverlay, &m_sSDLRect);
        }
    }
}

void SDL_VideoDevice::UpdateVideoWindow(void* pWindow, int nWidth, int nHeight)
{
    bool surfaceChange = (m_pUserWnd != pWindow);
    bool overlayChange = (m_nUserWndWidth != nWidth || m_nUserWndHeight != nHeight);
    if (surfaceChange)
    {
        EC_U32 nFlag = 0;
        overlayChange = true;
        m_pUserWnd = pWindow;
        SDL_FreeSurface(m_pSDLSurface);
        if (m_pUserWnd == NULL) nFlag = SDL_FULLSCREEN;
        m_pSDLSurface = SDL_SetVideoMode(m_nDeviceScreenWidth, m_nDeviceScreenHeight, 0, nFlag);
    }
    if (surfaceChange || overlayChange)
    {
        m_nUserWndWidth = nWidth;
        m_nUserWndHeight = nHeight;
        SetEqualScaling(&m_sSDLRect);
        SDL_FreeYUVOverlay(m_pSDLOverlay);
        m_pVideoFrameScale->UpdateScaleOutput(m_sSDLRect.w, m_sSDLRect.h);
        m_pSDLOverlay = SDL_CreateYUVOverlay(m_sSDLRect.w,
                                             m_sSDLRect.h,
                                             SDL_IYUV_OVERLAY, m_pSDLSurface);
        EraseVideoRim();
    }
}

void SDL_VideoDevice::SetEqualScaling(SDL_Rect *pRect)
{
    if (pRect == NULL) return;

    EC_U32 nTmpWidth = m_nUserWndWidth;
    EC_U32 nTmpHeight = m_nVideoHeight * nTmpWidth / m_nVideoWidth;
    pRect->x = 0;
    pRect->y = (m_nUserWndHeight - nTmpHeight) / 2;

    if (nTmpHeight > m_nUserWndHeight)
    {
        nTmpHeight = m_nUserWndHeight;
        nTmpWidth = m_nVideoWidth * nTmpHeight / m_nVideoHeight;
        pRect->y = 0;
        pRect->x = (m_nUserWndWidth - nTmpWidth) / 2;
    }
    EC_U32 xa = pRect->x % VIDEO_POSI_ALIGN;
    EC_U32 ya = pRect->y % VIDEO_POSI_ALIGN;
    EC_U32 wa = nTmpWidth % VIDEO_SIZE_ALIGN;
    EC_U32 ha = nTmpHeight % VIDEO_SIZE_ALIGN;

    pRect->x = pRect->x + xa;
    pRect->y = pRect->y + ya;
    pRect->w = nTmpWidth - wa;
    pRect->h = nTmpHeight - ha;
}

void SDL_VideoDevice::EraseVideoRim()
{
    SDL_Rect clipRectd;
    clipRectd.x = 0;
    clipRectd.y = 0;
    clipRectd.w = m_nUserWndWidth;
    clipRectd.h = m_nUserWndHeight;
    SDL_FillRect(m_pSDLSurface, &clipRectd, 0x0);
}
