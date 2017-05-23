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
 * EPlayerAPI_IMP.cpp
 * This file for EasyPlayer [EPlayer] top layer API implemetaion
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#include "Player.h"
#include "EPlayerAPI.h"

static Player* gPlayer = NULL;

void Init()
{
    gPlayer = Player::Instance();
}

void Uninit()
{
    if (gPlayer)
    {
        delete gPlayer;
        gPlayer = NULL;
    }
}

int OpenMedia(char* pURL,
              void* pVideoWindow,
              unsigned int nWindowWidth,
              unsigned int nWindowHeight)
{
    return gPlayer->OpenMedia(pURL, pVideoWindow, nWindowWidth, nWindowHeight);
}

int CloseMedia()
{
    return gPlayer->CloseMedia();
}

int Play()
{
    return gPlayer->Play();
}

int Pause()
{
    return gPlayer->Pause();
}

int Seek(unsigned int nSeekPos)
{
    return gPlayer->Seek(nSeekPos);
}

int GetPlayingPos()
{
    return gPlayer->GetPlayingPos();
}

int GetBufferingPos()
{
    return gPlayer->GetBufferingPos();
}

EPlayerStatus GetPlayerStatus()
{
    return (EPlayerStatus)gPlayer->GetPlayerStatus();
}

void GetMeidaInfo(MediaInfo* pMediaInfo)
{
    if (pMediaInfo)
    {
        const MediaContext *pCtx = gPlayer->GetMeidaContext();
        if (pCtx != NULL)
        {
            pMediaInfo->hasAudio = pCtx->hasAudio;
            pMediaInfo->hasVideo = pCtx->hasVideo;
            pMediaInfo->nDuration = pCtx->nDuration;
            pMediaInfo->nVideoWidth = pCtx->nVideoWidth;
            pMediaInfo->nVideoHeight = pCtx->nVideoHeight;
            pMediaInfo->nAudioChannels = pCtx->nChannels;
            pMediaInfo->nAudioSampleRate = pCtx->nSampleRate;
        }
    }
}

void SetMessageNotifer(MediaNotifier* pNotifier)
{
    if (pNotifier)
    {
        gPlayer->SetCallback(pNotifier->pUserData, pNotifier->Callback);
    }
}

void ResizeVidoeScreen(unsigned int nWidth, unsigned int nHeight)
{
    if (nWidth > 0 && nHeight > 0)
    {
        gPlayer->ResizeVideoWindow(nWidth, nHeight);
    }
}

void UpdateVideoScreen(void* pScreen, unsigned int nWidth, unsigned int nHeight)
{
    if (pScreen && nWidth > 0 && nHeight > 0)
    {
        gPlayer->UpdateVideoWindow(pScreen, nWidth, nHeight);
    }
}

void InitEPlayerAPI(EPlayerAPI* pAPI)
{
    if (pAPI)
    {
        pAPI->Init = Init;
        pAPI->Uninit = Uninit;
        pAPI->OpenMedia = OpenMedia;
        pAPI->CloseMedia = CloseMedia;
        pAPI->Play = Play;
        pAPI->Pause = Pause;
        pAPI->Seek = Seek;
        pAPI->GetMeidaInfo = GetMeidaInfo;
        pAPI->GetPlayingPos = GetPlayingPos;
        pAPI->GetPlayerStatus = GetPlayerStatus;
        pAPI->GetBufferingPos = GetBufferingPos;
        pAPI->SetMessageNotifer = SetMessageNotifer;
        pAPI->ResizeVidoeScreen = ResizeVidoeScreen;
        pAPI->UpdateVideoScreen = UpdateVideoScreen;
    }
}
