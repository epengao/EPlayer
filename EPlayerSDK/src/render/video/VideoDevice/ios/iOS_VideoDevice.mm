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
 * iOS_AudioDevice.cpp
 * This file is iOS_VideoDevice class implementation
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#import "VideoWindow.h"
#import "iOS_VideoDevice.h"

iOS_VideoDevice::iOS_VideoDevice()
:m_pUserWnd(nil)
,m_nVideoWidth(0)
,m_nVideoHeight(0)
,m_nUserWndWidth(0)
,m_nUserWndHeight(0)
{
}

iOS_VideoDevice::~iOS_VideoDevice()
{
}

int iOS_VideoDevice::Init(void* pVideoWindow,
                          int nWindowWidth,
                          int nWindowHeight,
                          MediaContext* pMediaContext)
{
    m_nUserWndWidth = nWindowWidth;
    m_nUserWndHeight = nWindowHeight;
    m_nVideoWidth = pMediaContext->nVideoWidth;
    m_nVideoHeight = pMediaContext->nVideoHeight;
    m_pUserWnd = (__bridge VideoWindow*)pVideoWindow;
    [m_pUserWnd setVidowSize:m_nVideoWidth height:m_nVideoHeight];
    return 0;
}

void iOS_VideoDevice::Uninit()
{
    m_pUserWnd = nil;
    m_nVideoWidth = 0;
    m_nVideoHeight = 0;
    m_nUserWndWidth = 0;
    m_nUserWndHeight = 0;
}

void iOS_VideoDevice::Flush()
{
}

void iOS_VideoDevice::DrawFrame(VideoFrame *pFrame)
{
    if(pFrame && (m_pUserWnd != nil))
    {
        AVFrame *frame = pFrame->pAVFrame;
        if(frame)
        {
            [m_pUserWnd drawYUV:frame->data[0] U:frame->data[1] V:frame->data[2]];
        }
    }
}

void iOS_VideoDevice::UpdateVideoWindow(void* pWindow, int nWidth, int nHeight)
{
    m_nUserWndWidth = nWidth;
    m_nUserWndHeight = nHeight;
    m_pUserWnd = (__bridge VideoWindow*)pWindow;
    [m_pUserWnd setVidowSize:m_nVideoWidth height:m_nVideoHeight];
}
