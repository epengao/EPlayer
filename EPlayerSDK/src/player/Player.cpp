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
 * Player.cpp
 * This file for Player class implemetaion
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#include "ECLog.h"
#include "ECUtil.h"
#include "Player.h"
#include "ECAutoLock.h"

/* Singleton */
Player* Player::m_pInstance = NULL;

Player* Player::Instance()
{
    if (m_pInstance == NULL)
    {
        m_pInstance = new Player();
    }
    return m_pInstance;
}

Player::Player()
:m_AEOS(true)
,m_VEOS(true)
,m_pVideoWnd(NULL)
,m_nVideoWndWidth(0)
,m_nVideoWndHeight(0)
,m_nPlayerStatus(PlayerStatus_Init)
{
    m_MsgNotifer.Callback = NULL;
    m_MsgNotifer.pUserData = NULL;

    m_pMediaCtrl = new MediaCtrl();
    m_pMessageCtrl = MessageHub::GetInstance();
    m_pMessageCtrl->SetNotifer(this, MessageHandler);
}

Player::~Player()
{
    if (m_pMediaCtrl) delete m_pMediaCtrl;
    if (m_pMessageCtrl) delete m_pMessageCtrl;
    m_pInstance = NULL;
}

int Player::OpenMedia(const char* pMeidaPath,
                      void* pVideoWindow,
                      EC_U32 nWindowWidth,
                      EC_U32 nWindowHeight)
{
    m_pVideoWnd = pVideoWindow;
    m_nVideoWndWidth = nWindowWidth;
    m_nVideoWndHeight = nWindowHeight;

    int ret = CheckStatusSwitch(PlayerStatus_Opened);
    if(Player_Err_None == ret)
    {
        ECAutoLock Lock(&m_mtxStatus);
        ret = m_pMediaCtrl->OpenMedia(pMeidaPath, pVideoWindow,
                                      m_nVideoWndWidth, m_nVideoWndHeight);
        if(ret == Player_Err_None)
        {
            const MediaContext* pMCtx = m_pMediaCtrl->GetMediaContext();
            if(pMCtx->hasAudio) m_AEOS = false;
            if(pMCtx->hasVideo) m_VEOS = false;
            m_nPlayerStatus = PlayerStatus_Opened;
        }
        else
        {
            ret = OpenMediaResultCheck(ret);
        }
    }
    return ret;
}

int Player::CloseMedia()
{
    int ret = CheckStatusSwitch(PlayerStatus_Closed);
    if(Player_Err_None == ret)
    {
        ECAutoLock Lock(&m_mtxStatus);
        m_AEOS = true;
        m_VEOS = true;
        m_pMediaCtrl->CloseMedia();
        m_nPlayerStatus = PlayerStatus_Closed;
    }
    return ret;
}

int Player::Play(unsigned int nStartPos)
{
    int ret = CheckStatusSwitch(PlayerStatus_Playing);
    if(Player_Err_None == ret)
    {
        ECAutoLock Lock(&m_mtxStatus);
        m_pMediaCtrl->Play(nStartPos);
        m_nPlayerStatus = PlayerStatus_Playing;
    }
    return ret;
}

int Player::Pause()
{
    int ret = CheckStatusSwitch(PlayerStatus_Paused);
    if(Player_Err_None == ret)
    {
        ECAutoLock Lock(&m_mtxStatus);
        m_pMediaCtrl->Pause();
        m_nPlayerStatus = PlayerStatus_Paused;
    }
    return ret;
}

int Player::Seek(unsigned int nSeekPos)
{
    int ret = CheckStatusSwitch(PlayerStatus_Seeking);
    if(Player_Err_None == ret)
    {
        ECAutoLock Lock(&m_mtxStatus);
        PlayerStatus statusBak = m_nPlayerStatus;
        m_nPlayerStatus = PlayerStatus_Seeking;
        if (nSeekPos > m_pMediaCtrl->GetDuration())
            ret = Player_Err_OutOfDuraion;
        else
        {
            m_pMediaCtrl->Pause();
            ret = m_pMediaCtrl->Seek(nSeekPos);
            if (statusBak == PlayerStatus_Playing)
            {
                m_pMediaCtrl->Play();
            }
            if(ret != Player_Err_None)
            {
                ret = Player_Err_SeekFailed;
            }
        }
        m_nPlayerStatus = statusBak;
    }
    return ret;
}

int Player::GetDuration()
{
    int duration = -1;
    const MediaContext* ctx = GetMeidaContext();
    if(ctx)
    {
        duration = ctx->nDuration;
    }
    return duration;
}

int Player::GetPlayingPos()
{
    int retPos = 0;
    if(m_nPlayerStatus == PlayerStatus_Paused ||
       m_nPlayerStatus == PlayerStatus_Playing)
    {
        retPos = m_pMediaCtrl->GetPlayingPos();
    }
    return retPos;
}

int Player::GetBufferingPos()
{
    return (int)m_pMediaCtrl->GetBufferingPos();
}

PlayerStatus Player::GetPlayerStatus()
{
    return m_nPlayerStatus;
}

const MediaContext* Player::GetMeidaContext()
{
    return m_pMediaCtrl->GetMediaContext();
}

char* GetParam(const char* pKey)
{
    return ECGetEnv(pKey);
}

void SetParam(const char* pKey, const char* pVal)
{
    ECSetEnv(pKey, pVal);
}

void Player::SetCallback(void* pUserData, void(*callback)(void*, unsigned int msg))
{
    m_MsgNotifer.Callback = callback;
    m_MsgNotifer.pUserData = pUserData;
}

void Player::ResizeVideoWindow(EC_U32 nWidth, EC_U32 nHeight)
{
    m_nVideoWndWidth = nWidth;
    m_nVideoWndHeight = nHeight;
    m_pMediaCtrl->UpdateVideoWindow(m_pVideoWnd, nWidth, nHeight);
}

void Player::UpdateVideoWindow(void* pVideoWindow, EC_U32 nWidth, EC_U32 nHeight)
{
    m_pVideoWnd = pVideoWindow;
    m_nVideoWndWidth = nWidth;
    m_nVideoWndHeight = nHeight;
    m_pMediaCtrl->UpdateVideoWindow(pVideoWindow, nWidth, nHeight);
}

/* Private Method */
int Player::OpenMediaResultCheck(int retVal)
{
    int ret = Player_Err_OpenFail;
    if(retVal == Audio_Dec_Err_OpenFaild)
        ret = Player_Err_OpenAudioDevFail;
    else if(retVal == Audio_Render_Err_DeviceErr)
        ret = Player_Err_OpenAudioDevFail;
    else if(retVal == Video_Render_Err_DeviceErr)
        ret = Player_Err_OpenAudioDevFail;
    else if(retVal == Video_Render_Err_DeviceErr)
        ret = Player_Err_OpenVideoDevFail;
    return ret;
}

int Player::CheckStatusSwitch(PlayerStatus nNextStatus)
{
    int ret = Player_Err_UnKnown;
    switch (nNextStatus)
    {
        case PlayerStatus_Opened:
        {
            if((m_nPlayerStatus == PlayerStatus_Init) ||
               (m_nPlayerStatus == PlayerStatus_Closed) )
            {
                ret = Player_Err_None;
            }
            else if(m_nPlayerStatus == PlayerStatus_Unknown)
            {
                ret = Player_Err_NotInited;
            }
            else
            {
                ret = Player_Err_MediaStillAct;
            }
        }
        break;
        case PlayerStatus_Playing:
        {
            if((m_nPlayerStatus == PlayerStatus_Opened) ||
               (m_nPlayerStatus == PlayerStatus_Paused) )
            {
                ret = Player_Err_None;
            }
            else if(m_nPlayerStatus == PlayerStatus_Playing)
            {
                ret = Player_Err_MediaPlaying;
            }
            else
            {
                ret = Player_Err_NoMediaOpened;
            }
        }
        break;
        case PlayerStatus_Seeking:
        {
            if((m_nPlayerStatus == PlayerStatus_Paused) ||
               (m_nPlayerStatus == PlayerStatus_Playing) )
            {
                ret = Player_Err_None;
            }
            else if(m_nPlayerStatus == PlayerStatus_Seeking)
            {
                ret = Player_Err_MediaSeeking;
            }
            else
            {
                ret = Player_Err_NoMeidaActive;
            }
        }
        break;
        case PlayerStatus_Paused:
        {
            if((m_nPlayerStatus == PlayerStatus_Seeking)||
               (m_nPlayerStatus == PlayerStatus_Playing)||
               (m_nPlayerStatus == PlayerStatus_Paused) )
            {
                ret = Player_Err_None;
            }
            else
            {
                ret = Player_Err_NoMediaPlaying;
            }
        }
        break;
        case PlayerStatus_Closed:
        {
            if((m_nPlayerStatus == PlayerStatus_Opened) ||
               (m_nPlayerStatus == PlayerStatus_Paused) ||
               (m_nPlayerStatus == PlayerStatus_Seeking)||
               (m_nPlayerStatus == PlayerStatus_Playing) )
            {
                ret = Player_Err_None;
            }
            else
            {
                ret = Player_Err_NoMeidaActive;
            }
        }
        break;
        default: break;
    }
    return ret;
}

void Player::MessageHandler(void* pUserData, unsigned int msg)
{
    bool dropMsg = false;
    if(pUserData)
    {
        Player* pSelf = (Player*)pUserData;
        switch(msg)
        {
            case PlayerMessage_Audio_PlayFinished:
                pSelf->m_AEOS = true;
                break;
            case PlayerMessage_Video_PlayFinished:
                pSelf->m_VEOS = true;
                break;
            case PlayerMessage_PlayStart:
                if(pSelf->m_nPlayerStatus != PlayerStatus_Playing)
                    dropMsg = true;
            default: break;
        }
        if(pSelf->m_AEOS && pSelf->m_VEOS)
        {
            pSelf->m_pMediaCtrl->Pause();
            msg = PlayerMessage_PlayStop;
        }

        /* send msg to User layer */
        if(dropMsg) return;
        if(pSelf->m_MsgNotifer.Callback)
        {
            void* pNotifyUserData = pSelf->m_MsgNotifer.pUserData;
            pSelf->m_MsgNotifer.Callback(pNotifyUserData, msg);
        }
    }
}
