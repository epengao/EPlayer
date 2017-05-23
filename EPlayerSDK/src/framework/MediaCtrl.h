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
 * MediaCtrl.h
 * This file for Media Ctrl define.
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef MEDIA_CTRL_H
#define MEDIA_CTRL_H

#include "MediaClock.h"
#include "MediaSource.h"
#include "AudioDecoder.h"
#include "VideoDecoder.h"
#include "AudioRender.h"
#include "VideoRender.h"

typedef enum
{
    MediaCtrlStatus_Play,
    MediaCtrlStatus_Pause,
    MediaCtrlStatus_NoAct,
    MediaCtrlStatus_Unknown = -1,
} MediaCtrlStatus;

class MediaCtrl
{
public:
    MediaCtrl();
    ~MediaCtrl();

    int OpenMedia(const char* pMeidaPath,
                  void* pVideoWindow = NULL,
                  EC_U32 nWindowWidth = 0,
                  EC_U32 nWindowHeight = 0);
    void CloseMedia();

    void Play(unsigned int nStartPos = 0);
    void Pause();
    int  Seek(EC_U32 nSeekPos);
    int  GetDuration();
    int  GetPlayingPos();
    int  GetBufferingPos();
    MediaContext* GetMediaContext();
    void UpdateVideoWindow(void* pWindow, EC_U32 nWidth, EC_U32 nHeight);

private:
    bool                m_HasA;
    bool                m_HasV;
    MediaClock*         m_pClk;
    MediaSource*        m_pSrc;
    AudioDecoder*       m_pADec;
    VideoDecoder*       m_pVDec;
    AudioRender*        m_pARnd;
    VideoRender*        m_pVRnd;
    MediaCtrlStatus     m_nStatus;
};

#endif /* MEDIA_CTRL_H */
