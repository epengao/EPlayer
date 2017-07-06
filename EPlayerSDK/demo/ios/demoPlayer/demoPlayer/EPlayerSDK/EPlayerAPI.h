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

#import <Foundation/Foundation.h>
#if TARGET_OS_IPHONE
#include <UIKit/UIKit.h>
#elif TARGET_OS_MAC
#import <Cocoa/Cocoa.h>
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
typedef NS_ENUM(NSUInteger, EPlayerMessage)
{
    EPlayer_Msg_PlayStart            = 1,
    EPlayer_Msg_PlayStop             = 2,
    EPlayer_Msg_NetworkError         = 3,
    EPlayer_Msg_BufferingStart       = 4,
    EPlayer_Msg_BufferingStop        = 5,
    EPlayer_Msg_Audio_PlayFinished   = 6,
    EPlayer_Msg_Video_PlayFinished   = 7,
    EPlayer_Msg_Undefine        = 0xFFFF,
};

/* EPlayer Status Define */
typedef NS_ENUM(NSUInteger, EPlayerStatus)
{
    EPlayerStatus_Init       = 0,
    EPlayerStatus_Opened     = 1,
    EPlayerStatus_Playing    = 2,
    EPlayerStatus_Seeking    = 3,
    EPlayerStatus_Paused     = 4,
    EPlayerStatus_Closed     = 5,
    EPlayerStatus_Unknown = 0xFF,
};

#if TARGET_OS_IPHONE
@interface VideoWindow : UIView
- (id)initWithFrame:(CGRect)frame;
- (id)initWithCoder:(NSCoder*)aDecoder;
@end
#elif TARGET_OS_MAC
@interface VideoWindow : NSObject
+ (VideoWindow*)createVideoWindowFrom:(NSWindow*)window;
@end
#endif

/*
 * MeidaInfo
 * NOTE: duration use ms
 */
@interface MediaInfo : NSObject
  @property (nonatomic, assign) BOOL seekable;
  @property (nonatomic, assign) BOOL hasAudio;
  @property (nonatomic, assign) BOOL hasVideo;
  @property (nonatomic, assign) NSUInteger duration;
  @property (nonatomic, assign) NSUInteger videoWidth;
  @property (nonatomic, assign) NSUInteger videoHeight;
  @property (nonatomic, assign) NSInteger videoRotation;
  @property (nonatomic, assign) NSUInteger audioChannels;
  @property (nonatomic, assign) NSUInteger audioSampleRate;
@end

/*
 * EPlayer message notify delegate
 * @msg:   This is the EasyPlayer [EPlayer] notify message to user
 *         layer routine. msg is the SDK layer event defined
 *         by EPlayerMessage.
 */
@protocol EPlayerSdkDelegate <NSObject>
@optional - (void) handleEPlayerMsg: (EPlayerMessage)msg;
@end

/* Export API:
 * EPlayer API call procedure:
 *         {New media ---[       play opration       ]---- Finished}
 * Init -> {OpenMedia -> [   Play |  Pause  | Seek   ] -> CloseMeida} -> Uninit
 */
@interface EPlayerAPI : NSObject
+ (EPlayerAPI*) sharedEPlayerAPI;
/*
 * API: openMeida
 * @pMediaPath: Local file/Network URL
 * @videoWindow: Video output Window
                 Mac create it with NSWindow, iOS create it with UIView
 * @windowWidth: Video output window width
 * @windowHeight: Video output window height
 * API return: If open a media OK return 0, else return Err value.
 */
- (NSInteger) openMediaPath:(NSString*)path
                videoWindow:(VideoWindow*)window
                windowWidth:(NSUInteger)width
               windowHeight:(NSUInteger)height;
/*
 * API: closeMedia
 * Close current media and stop it.
 */
-(NSInteger) closeMedia;
/*
 * API: play
 * Start play an opened media.
 */
- (NSInteger) play;
/*
 * API: pause
 * Start pause playing media.
 */
- (NSInteger) pause;
/*
 * API: seek
 * Seek a playing/paused media.
 * @seekPos: seek target position, time unit is ms.
 */
- (NSInteger) seek:(NSUInteger)seekPos;
/*
 * API: getPlayingPos
 * API return: Current playing time(ms) [0 ~ MediaInfo.duration].
 */
- (NSInteger) getPlayingPos;
/*
 * API: getBufferingPos (used when play network URL)
 * API return: Current buffering position(ms) [0 ~ MediaInfo.nDuration].
 */
- (NSInteger) getBufferingPos;
/*
 * API: getPlayerStatus
 * API return: Current player status that all defined by EPlayerStatus.
 */
- (EPlayerStatus) getPlayerStatus;
/*
 * API: GetMeidaInfo
 * API return: return MediaInfo* for getting some media information.
 */
- (MediaInfo*) getMeidaInfo;
/*
 * API: hasMediaActived
 * API return: If there's a media opened/playing/pause/seeking,
               return YES, else return NO.
 */
- (BOOL) hasMediaActived;
/*
 * API: resizeVidoeWindow
 * @width:  new video output window width
 * @height: new video output window height
 * This API for resize current play video size but not changed the window handler.
 */
- (void) resizeVidoeWindow:(NSInteger)width
                    height:(NSInteger)height;
/*
 * API: updateVideoWindow
 * @videoWindow: Video output Window
                 Mac with NSWindow, iOS with UIImage
 * @width: new video output window width
 * @hight: new video output window height
 * This API for video output window changed, from current window to another.
 */
- (void) updateVideoWindow:(VideoWindow*)window
               windowWidth:(NSUInteger)width
              windowHeight:(NSUInteger)height;
/*
 * EPlayerSDK message handler -> EPlayerSdkDelegate
 */
@property (nonatomic, assign) id<EPlayerSdkDelegate> msgHandler;
@end
