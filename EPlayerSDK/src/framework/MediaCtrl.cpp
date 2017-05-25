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
 * MediaCtrl.cpp
 * This file for MediaCtrl class implemetaion
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#include "ECLog.h"
#include "MediaCtrl.h"

MediaCtrl::MediaCtrl()
:m_HasA(false)
,m_HasV(false)
,m_nStatus(MediaCtrlStatus_NoAct)
{
    m_pClk = new MediaClock();
    m_pSrc = new MediaSource();
    m_pADec = new AudioDecoder();
    m_pVDec = new VideoDecoder();
    m_pARnd = new AudioRender(m_pClk);
    m_pVRnd = new VideoRender(m_pClk);
}

MediaCtrl::~MediaCtrl()
{
    if (m_pARnd) delete m_pARnd;
    if (m_pVRnd) delete m_pVRnd;
    if (m_pADec) delete m_pADec;
    if (m_pVDec) delete m_pVDec;
    if (m_pSrc)  delete m_pSrc;
    if (m_pClk)  delete m_pClk;
}

int MediaCtrl::OpenMedia(const char* pMeidaPath,
                         void* pVideoWindow,
                         EC_U32 nWindowWidth,
                         EC_U32 nWindowHeight)
{
    int ret = m_pSrc->OpenMedia(pMeidaPath);
    if (ret == 0)
    {
        MediaContext* pMediaCtx = m_pSrc->GetMediaContext();
        if(!pMediaCtx || (!pMediaCtx->hasAudio && !pMediaCtx->hasVideo))
        {
            m_pSrc->CloseMedia();
            return Source_Err_NoMediaTrack;
        }

        /* create audio stream chain */
        if(pMediaCtx->hasAudio)
        {
            SourceAudioPort* pSrcAPort = m_pSrc->GetMediaSourceAudioPort();
            ret = m_pADec->OpenDecoder(pMediaCtx, pSrcAPort);
            if (ret == 0)
            {
                AudioDecPort* pDecPort = m_pADec->GetAudioDecPort();
                ret = m_pARnd->OpenDevice(pMediaCtx, pDecPort);
                if (ret != 0)
                {
                    m_pARnd->CloseDevice();
                    m_pADec->CloseDecoder();
                    ECLogW("AudioRender::OpenDevice() failed...!");
                }
                else
                {
                    m_HasA = true;
                }
            }
            else
            {
                m_pADec->CloseDecoder();
                ECLogW("AudioDecoder::OpenDecoder() failed...!");
            }
        }
        if(ret)
        {
            m_pSrc->CloseMedia();
            return ret;
        }

        /* create video stream chain */
        if(pMediaCtx->hasVideo)
        {
            if(pVideoWindow && (nWindowWidth > 0) && (nWindowHeight > 0))
            {
                SourceVideoPort* pSrcVPort = m_pSrc->GetMediaSourceVideoPort();
                ret = m_pVDec->OpenDecoder(pMediaCtx, pSrcVPort);
                if (ret == 0)
                {
                    VideoDecPort* pVDecPort = m_pVDec->GetVideoDecPort();
                    ret = m_pVRnd->OpenDevice(pVideoWindow, nWindowWidth, nWindowHeight, pMediaCtx, pVDecPort);
                    if (ret != 0)
                    {
                        m_pVRnd->CloseDevice();
                        m_pVDec->CloseDecoder();
                        ECLogW("VideoRender::OpenDevice() failed...!");
                    }
                    else
                    {
                        m_HasV = true;
                    }
                }
                else
                {
                    m_pVDec->CloseDecoder();
                    ECLogW("VideoDecoder::OpenDecoder() failed...!");
                }
            }
            else
            {
                ret = Video_Render_Err_DeviceErr;
                ECLogW("VideoRender::OpenDevice() failed...Bad Screem Param!");
            }
        }
        if(ret)
        {
            if(m_HasA)
            {
                m_HasA = false;
                if(m_pARnd) m_pARnd->CloseDevice();
                if(m_pADec) m_pADec->CloseDecoder();
            }
            m_pSrc->CloseMedia();
            return ret;
        }
    }
    else
    {
        m_pSrc->CloseMedia();
        ECLogW("MediaSource::OpenMedia() failed...!");
    }
    return ret;
}

void MediaCtrl::CloseMedia()
{
    if(m_HasA)
    {
        if(m_pARnd) m_pARnd->CloseDevice();
        if(m_pADec) m_pADec->CloseDecoder();
    }
    if(m_HasV)
    {
        if(m_pVRnd) m_pVRnd->CloseDevice();
        if(m_pVDec) m_pVDec->CloseDecoder();
    }
    if(m_pSrc) m_pSrc->CloseMedia();
    if(m_pClk) m_pClk->Reset();
    m_nStatus = MediaCtrlStatus_NoAct;
}

void MediaCtrl::Play(unsigned int nStartPos)
{
    m_pSrc->Run();
	m_pClk->Run();
    if(nStartPos > 0)
    {
        m_pSrc->Seek(nStartPos);
    }
    if(m_HasA && m_pARnd) m_pARnd->Run();
    if(m_HasV && m_pVRnd) m_pVRnd->Run();
    m_nStatus = MediaCtrlStatus_Play;
}

void MediaCtrl::Pause()
{
    if(m_pSrc)  m_pSrc->Pause();
    if(m_HasA && m_pARnd) m_pARnd->Pause();
    if(m_HasV && m_pVRnd) m_pVRnd->Pause();
    if(m_pClk) m_pClk->Pause();
    m_nStatus = MediaCtrlStatus_Pause;
}

int MediaCtrl::Seek(EC_U32 nSeekPos)
{
    bool fastSeek = true;
    m_pClk->SetMediaTime(nSeekPos);
    int ret = m_pSrc->Seek(nSeekPos);
    if(Source_Err_None == ret)
    {
        if(m_HasA)
        {
            if(m_pADec) m_pADec->Flush();
            if(m_pARnd) m_pARnd->Seek(nSeekPos, fastSeek);
        }
        if(m_HasV)
        {
            if(m_pVDec) m_pVDec->Flush();
            if(m_pVRnd) m_pVRnd->Seek(nSeekPos, fastSeek);
        }
    }
    return ret;
}

int MediaCtrl::GetDuration()
{
    int ret = 0;
    if(m_pSrc)
    {
        ret = m_pSrc->GetDuration();
    }
    return ret;
}

int MediaCtrl::GetPlayingPos()
{
    int ret = -1;
    if(m_pClk)
    {
        int duration = GetDuration();
        int currtime = (int)m_pClk->GetClockTime();
        ret = (currtime > duration) ? duration : currtime;
    }
    return ret;
}

int MediaCtrl::GetBufferingPos()
{
    int ret = -1;
    if(m_pClk)
    {
        ret = (int)m_pSrc->GetBufferingTime();
    }
    return ret;
}

MediaContext* MediaCtrl::GetMediaContext()
{
    MediaContext* pRet = NULL;
    if(m_pSrc)
    {
        pRet = m_pSrc->GetMediaContext();
    }
    return pRet;
}

void MediaCtrl::UpdateVideoWindow(void* pWindow, EC_U32 nWidth, EC_U32 nHeight)
{
    if(m_pVRnd)
    {
        m_pVRnd->UpdateVideoWindow(pWindow, nWidth, nHeight);
    }
}
