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
 * EPlayerAPI.h
 * This file for EasyPlayer [EPlayer] API Define
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef E_PLAYER_API_H
#define E_PLAYER_API_H

#ifdef __cplusplus
extern "C" {
#endif

/* Return Err Define */
#define EPlayer_Err_None                 0x00000000
#define EPlayer_Err_NotInited            0x0000A001
#define EPlayer_Err_NoMediaOpened        0x0000A002
#define EPlayer_Err_NoMediaPlaying       0x0000A003
#define EPlayer_Err_NoMeidaActive        0x0000A004
#define EPlayer_Err_MediaStillAct        0x0000A005
#define EPlayer_Err_MediaPlaying         0x0000A006
#define EPlayer_Err_MediaSeeking         0x0000A007
#define EPlayer_Err_OpenFail             0x0000A008
#define EPlayer_Err_SeekFailed           0x0000A009
#define EPlayer_Err_OutOfDuraion         0x0000A00A
#define EPlayer_Err_NoMediaContent       0x0000A00B
#define EPlayer_Err_OpenAudioDecFail     0x0000A00C
#define EPlayer_Err_OpenVideoDecFail     0x0000A00D
#define EPlayer_Err_OpenAudioDevFail     0x0000A00E
#define EPlayer_Err_OpenVideoDevFail     0x0000A00F
#define EPlayer_Err_UnKnown              0xF000AFFF

/* Notify Message Define */
typedef enum
{
    EPlayer_Msg_PlayStart            = 1,
    EPlayer_Msg_PlayStop             = 2,
    EPlayer_Msg_NetworkError         = 3,
    EPlayer_Msg_Video_BufferingStart = 4,
    EPlayer_Msg_Video_BufferingStop  = 5,
    EPlayer_Msg_Audio_BufferingStart = 6,
    EPlayer_Msg_Audio_BufferingStop  = 7,
    EPlayer_Msg_Audio_PlayFinished   = 8,
    EPlayer_Msg_Video_PlayFinished   = 9,
    EPlayer_Msg_Undefine        = 0xFFFF,
} EPlayerMessage;

/* EPlayer Status Define */
typedef enum
{
    EPlayerStatus_Init,
    EPlayerStatus_Opened,
    EPlayerStatus_Playing,
    EPlayerStatus_Seeking,
    EPlayerStatus_Paused,
    EPlayerStatus_Closed,
    EPlayerStatus_Unknown = -1,
} EPlayerStatus;

/*
 * MeidaInfo
 * NOTE: duration use ms
 */
typedef struct
{
    bool hasAudio;
    bool hasVideo;
    unsigned int nDuration;
    unsigned int nVideoWidth;
    unsigned int nVideoHeight;
    unsigned int nAudioChannels;
    unsigned int nAudioSampleRate;
} MediaInfo;

/*
 * EPlayer notify callback
 * @pUserData: User defined data, when Callback
 *             comes take it as parameter pUserData.
 * @Callback: This is the EasyPlayer [EPlayer] notify message to
 *            user layer callback routine. msg is the notify identify.
 */
typedef struct
{
    void *pUserData;
    void (*Callback)(void *pUserData, unsigned int msg);
} MediaNotifier;

/* Export API:
 * EPlayer API call procedure:
 *         {New media ---[       play opration       ]---- Finished}
 * Init -> {OpenMedia -> [   Play |  Pause  | Seek   ] -> CloseMeida} -> Uninit
 */
typedef struct
{
    void (*Init)();
    void (*Uninit)();
    /*
     * API: OpenMeida
     * @pMediaPath: Local file/Network URL
     * @pVideoWindow: Video output window handler:
     *                Mac platform is a NSWindow object
     *                Win platform is a hWnd created by CreateWindow/CreateWindowEx
     * @nWindowWidth:  Video output window width
     * @nWindowHeight: Video output window height
     * API return: OpenMeida OK return 0, else return Err value.
     */
    int  (*OpenMedia)(char* pMediaPath,
                      void* pVideoWindow,
                      unsigned int nWindowWidth,
                      unsigned int nWindowHeight);
    int  (*CloseMedia)();
    int  (*Play)();
    int  (*Pause)();
    int  (*Seek)(unsigned int nSeekPos/*ms*/);
    /*
     * API: GetPlayingPos
     * API return: Current playing time(ms) [0 ~ MediaInfo.nDuration].
     */
    int  (*GetPlayingPos)();
    /*
     * API: GetBufferingPos (used when play network URL)
     * API return: Current buffering position(ms) [0 ~ MediaInfo.nDuration].
     */
    int  (*GetBufferingPos)();
    /*
    * API: GetPlayerStatus
    * API return: Current player status that all defined by EPlayerStatus.
    */
    EPlayerStatus (*GetPlayerStatus)();
    /*
    * API: GetMeidaInfo
    * @pMediaInfo: a pointer to MediaInfo, get some media information.
    */
    void (*GetMeidaInfo)(MediaInfo* pMediaInfo);
    /*
     * API: SetMessageNotifer
     * @pNotifier: a pointer to MediaNotifier.
     * This API set a notifier for low layer SDK moduls set some playback message to
     * Application layer, all the messages are defined in EPlayerNotifyMsg.
     */
    void (*SetMessageNotifer)(MediaNotifier* pNotifier);
    /*
    * API: ResizeVidoeScreen
    * @nWidth:  new video output window width
    * @nHeight: new video output window height
    * This API for resize current play video size but not changed the window handler.
    */
    void (*ResizeVidoeScreen)(unsigned int nWidth, unsigned int nHeight);
    /*
    * API: UpdateVideoScreen
    * @pScreen: new video output handler:
    *           Mac platform is a NSWindow object
    *           Win platform is a hWnd created by CreateWindow/CreateWindowEx
    * @nWidth:  new video output window width
    * @nHeight: new video output window height
    * This API for video output screen changed, from current window to another.
    */
    void (*UpdateVideoScreen)(void* pScreen, unsigned int nWidth, unsigned int nHeight);
} EPlayerAPI;

/*
 * Export Define:
 * API Entry Point
*/
#ifdef WIN32
     __declspec (dllexport) void InitEPlayerAPI(EPlayerAPI* pAPI);
#elif defined LINUX
     __attribute__ (visibility("default")) void InitEPlayerAPI(EPlayerAPI* pAPI);
#else
     void InitEPlayerAPI(EPlayerAPI* pAPI);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* E_PLAYER_API_H */
