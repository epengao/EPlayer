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
 * Mac_AudioDevice.cpp
 * This file is Mac_VideoDevice class implementation
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#include "Mac_VideoDevice.h"

Mac_VideoDevice::Mac_VideoDevice()
{
    m_pRndDev = new SDL2_VideoDevice();
}

Mac_VideoDevice::~Mac_VideoDevice()
{
    delete m_pRndDev;
}

int Mac_VideoDevice::Init(void* pVideoWindow,
                          int nWindowWidth,
                          int nWindowHeight,
                          MediaContext* pMediaContext)
{

    return m_pRndDev->Init(pVideoWindow, nWindowWidth,
                           nWindowHeight, pMediaContext);
}

void Mac_VideoDevice::Uninit()
{
    m_pRndDev->Uninit();
}

void Mac_VideoDevice::Flush()
{
    m_pRndDev->Flush();
}

void Mac_VideoDevice::DrawFrame(VideoFrame *pFrame)
{
    if(pFrame) m_pRndDev->DrawFrame(pFrame);
}

void Mac_VideoDevice::UpdateVideoWindow(void* pWindow, int nWidth, int nHeight)
{
    m_pRndDev->UpdateVideoWindow(pWindow, nWidth, nHeight);
}
