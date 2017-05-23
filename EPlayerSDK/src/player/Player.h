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
 * Player.h
 * This file for Meida Player class define.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef PLAYER_H
#define PLAYER_H

#include "ECType.h"
#include "ECMutex.h"
#include "MediaCtrl.h"
#include "MessageHub.h"

/* Return Err Define */
#define Player_Err_None                 0x00000000
#define Player_Err_NotInited            0x0000A001
#define Player_Err_NoMediaOpened        0x0000A002
#define Player_Err_NoMediaPlaying       0x0000A003
#define Player_Err_NoMeidaActive        0x0000A004
#define Player_Err_MediaStillAct        0x0000A005
#define Player_Err_MediaPlaying         0x0000A006
#define Player_Err_MediaSeeking         0x0000A007
#define Player_Err_OpenFail             0x0000A008
#define Player_Err_SeekFailed           0x0000A009
#define Player_Err_OutOfDuraion         0x0000A00A
#define Player_Err_NoMediaContent       0x0000A00B
#define Player_Err_OpenAudioDecFail     0x0000A00C
#define Player_Err_OpenVideoDecFail     0x0000A00D
#define Player_Err_OpenAudioDevFail     0x0000A00E
#define Player_Err_OpenVideoDevFail     0x0000A00F
#define Player_Err_UnKnown              0xF000AFFF

typedef enum
{
    PlayerStatus_Init,
    PlayerStatus_Opened,
    PlayerStatus_Playing,
    PlayerStatus_Seeking,
    PlayerStatus_Paused,
    PlayerStatus_Closed,
    PlayerStatus_Unknown = -1,
} PlayerStatus;

class Player
{
public:
    ~Player();
    static Player* Instance();
    int OpenMedia(const char* pMeidaPath,
                  void* pVideoWindow = NULL,
                  EC_U32 nWindowWidth = 0,
                  EC_U32 nWindowHeight = 0);
    int CloseMedia();

    int Play(EC_U32 nStartPos = 0);
    int Pause();
    int Seek(EC_U32 nSeekPos);
    int GetDuration();
    int GetPlayingPos();
    int GetBufferingPos();
    PlayerStatus GetPlayerStatus();
    const MediaContext* GetMeidaContext();
    char* GetParam(const char* pKey);
    void SetParam(const char* pKey, const char* pVal);
    void SetCallback(void* pUserData, void(*callback)(void*, EC_U32 msg));
    void ResizeVideoWindow(EC_U32 nWidth, EC_U32 nHeight);
    void UpdateVideoWindow(void* pVideoWindow, EC_U32 nWidth, EC_U32 nHeight);

private:
    /* Singleton */
    Player();
    Player(const Player &);
    Player& operator = (const Player &);
    static Player* m_pInstance;

private:
    int OpenMediaResultCheck(int retVal);
    int CheckStatusSwitch(PlayerStatus nNextStatus);
    static void MessageHandler(void* pUserData, EC_U32 msg);

private:
    bool         m_AEOS;
    bool         m_VEOS;
    void*        m_pVideoWnd;
    EC_U32       m_nVideoWndWidth;
    EC_U32       m_nVideoWndHeight;
    ECMutex      m_mtxStatus;
    MediaCtrl*   m_pMediaCtrl;
    MessageHub*  m_pMessageCtrl;
    MsgNotifier  m_MsgNotifer;
    PlayerStatus m_nPlayerStatus;
};

#endif /* PLAYER_H */
