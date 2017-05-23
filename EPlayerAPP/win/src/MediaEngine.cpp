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
* Inc., 59 Temple Place, Eite 330, Boston, MA  02111-1307  USA
*
* Project: EC < Enjoyable Coding >
*
* MediaEngine.cpp
* This file for MediaEngine class implementation.
*
* Eamil:   epengao@126.com
* Author:  Gao Peng
* Version: Intial first version.
* ****************************************************************
*/

#include <string.h>
#include "MediaEngine.h"

#define VIDEO_SIZE_AGIGN 8

MediaEngine::MediaEngine(void *pVideoWnd,
                         unsigned int nVideoWndWidth,
                         unsigned int nVideoWndHeight)
{
    m_sVideoOutput.pVideoWnd = pVideoWnd;
    m_sVideoOutput.nWndWidth = nVideoWndWidth;
    m_sVideoOutput.nWndHeight = nVideoWndHeight;
    //VideoScreenSizeAlign();

    memset(&m_sMediaInfo, 0, sizeof(MediaInfo));
    memset(&m_sMediaAPI, 0, sizeof(EPlayerAPI));

    InitEPlayerAPI(&m_sMediaAPI);
    if (m_sMediaAPI.Init)
    {
        m_sMediaAPI.Init();
    }
}

MediaEngine::~MediaEngine()
{
    if (m_sMediaAPI.Uninit)
    {
        m_sMediaAPI.Uninit();
    }
}

int  MediaEngine::OpenMedia(char *pMediaPath)
{
    int nRet = -1;
    if (m_sMediaAPI.OpenMedia)
    {
        nRet = m_sMediaAPI.OpenMedia(pMediaPath,
                                     m_sVideoOutput.pVideoWnd,
                                     m_sVideoOutput.nWndWidth,
                                     m_sVideoOutput.nWndHeight);
        m_sMediaAPI.GetMeidaInfo(&m_sMediaInfo);
    }
    return nRet;
}

void MediaEngine::Play()
{
    if (m_sMediaAPI.Play)
    {
        m_sMediaAPI.Play();
    }
}

void MediaEngine::Pause()
{
    if (m_sMediaAPI.Pause)
    {
        m_sMediaAPI.Pause();
    }
}

void MediaEngine::Seek(unsigned int nPos)
{
    if (m_sMediaAPI.Seek)
    {
        m_sMediaAPI.Seek(nPos);
    }
}

bool MediaEngine::HasVideo()
{
    return m_sMediaInfo.hasVideo;
}

void MediaEngine::Stop()
{
    if (m_sMediaAPI.CloseMedia)
    {
        m_sMediaAPI.CloseMedia();
    }
}

bool MediaEngine::IsPlaying()
{
    EPlayerStatus nStatus = EPlayerStatus_Unknown;
    if (m_sMediaAPI.GetPlayerStatus)
    {
        nStatus = m_sMediaAPI.GetPlayerStatus();
    }
    return (nStatus == EPlayerStatus_Playing);
}

bool MediaEngine::HasPlayMedia()
{
    EPlayerStatus nStatus = EPlayerStatus_Unknown;
    if (m_sMediaAPI.GetPlayerStatus)
    {
        nStatus = m_sMediaAPI.GetPlayerStatus();
    }
    return ((nStatus == EPlayerStatus_Paused) ||
            (nStatus == EPlayerStatus_Playing));
}

void MediaEngine::GetMediaInfo(MediaInfo *pMediaInfo)
{
    if (pMediaInfo)
    {
        memcpy(pMediaInfo, &m_sMediaInfo, sizeof(MediaInfo));
    }
}

void MediaEngine::SetMediaNotify(MediaNotifier *pNotify)
{
    if (pNotify && m_sMediaAPI.SetMessageNotifer)
    {
        m_sMediaAPI.SetMessageNotifer(pNotify);
    }
}

unsigned int MediaEngine::GetPlayPosition()
{
    unsigned int nPos = 0;
    if (m_sMediaAPI.GetPlayingPos)
    {
        nPos = m_sMediaAPI.GetPlayingPos();
    }
    return nPos;
}

unsigned int MediaEngine::GetMediaDuration()
{
    return m_sMediaInfo.nDuration;
}

void MediaEngine::UpdateVideoScreen(void *pVideoWnd, unsigned int nVideoWndWidth, unsigned int nVideoWndHeight)
{
    m_sVideoOutput.pVideoWnd = pVideoWnd;
    m_sVideoOutput.nWndWidth = nVideoWndWidth;
    m_sVideoOutput.nWndHeight = nVideoWndHeight;

    //VideoScreenSizeAlign();
    if (m_sMediaAPI.UpdateVideoScreen && m_sMediaInfo.hasVideo)
    {
        m_sMediaAPI.UpdateVideoScreen(pVideoWnd, nVideoWndWidth, nVideoWndHeight);
    }
}

void MediaEngine::VideoScreenSizeAlign()
{
    unsigned int w = m_sVideoOutput.nWndWidth;
    unsigned int h = m_sVideoOutput.nWndHeight;
    m_sVideoOutput.nWndWidth = w + (w%VIDEO_SIZE_AGIGN);
    m_sVideoOutput.nWndHeight = h + (h%VIDEO_SIZE_AGIGN);
}
