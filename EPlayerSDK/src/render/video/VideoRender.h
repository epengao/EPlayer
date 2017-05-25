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
 * VideoRender.h
 * This file for Video Frame data output to device screen.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef VIDEO_RENDER_H
#define VIDEO_RENDER_H

#include "ECType.h"
#include "ECMutex.h"
#include "MediaClock.h"
#include "VideoDecoder.h"
#include "VideoDecPort.h"
#include "VideoDeviceI.h"
#include "ECThreadDriver.h"

class VideoRender : public ECThreadWorkerI
{
public:
    VideoRender(MediaClock* pClock = NULL);
    ~VideoRender();

    void Run();
    void Pause();
    void Flush();
    void Seek(EC_U32 nPos, bool fastSeek = false);
    EC_U32 OpenDevice(void* pVideoWindow,
                      int nWindowWidth,
                      int nWindowHeight,
                      MediaContext* pMediaContext,
                      VideoDecPort* pVideoDecPort);
    void CloseDevice();
    void UpdateVideoWindow(void* pWindow, int nWidth, int nHeight);

private:
    /* ECThreadWorkerI */
    virtual void DoRunning();

private:
    bool            m_bEOS;
    VideoFrame      m_VFrame;
    ECMutex         m_mtxVideoDev;
    MediaClock*     m_pMedaiClock;
    VideoDecPort*   m_pDecoderPort;
    VideoDeviceI*   m_pVideoDevice;
    ECThreadDriver* m_pThreadDriver;
};

#endif /* VIDEO_RENDER_H */
