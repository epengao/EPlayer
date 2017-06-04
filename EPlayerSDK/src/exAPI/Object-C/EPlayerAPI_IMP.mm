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
 * EPlayerAPI_IMP.mm
 * Object-C [iOS / MacOS] Platform.
 * This file for EasyPlayer [EPlayer] top layer API implemetaion
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#import "Player.h"
#import "EPlayerAPI.h"

void NotifyHandler(void* pUserData, unsigned int msg);

@implementation MediaInfo
- (id)init
{
    self = [super init];
    if(self)
    {
        self.hasAudio = NO;
        self.hasVideo = NO;
        self.duration = 0;
        self.videoWidth = 0;
        self.videoHeight = 0;
        self.audioChannels = 0;
        self.audioSampleRate = 0;
    }
    return self;
}
@end

@interface EPlayerAPI ()
{
    Player* playerSDK;
    MediaInfo* mediaInfo;
}
@end
@implementation EPlayerAPI
- (id)init
{
    if(self = [super init])
    {
        mediaInfo = nil;
        playerSDK = Player::Instance();
        playerSDK->SetCallback((__bridge void*)self, NotifyHandler);
    }
    return self;
}

- (void)dealloc
{
    mediaInfo = nil;
    if(playerSDK) delete playerSDK;
    playerSDK = NULL;
}

+ (EPlayerAPI*) sharedEPlayerAPI
{
    static EPlayerAPI* instance;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = [[EPlayerAPI alloc]init];
    });
    return instance;
}

- (NSInteger) openMediaPath:(NSString*)path
                videoWindow:(void*)window
                windowWidth:(NSUInteger)width
               windowHeight:(NSUInteger)height
{
    const char* mediaPath = [path UTF8String];
    unsigned int wndWidth = (unsigned int)width;
    unsigned int wndHeight = (unsigned int)height;
    return playerSDK->OpenMedia(mediaPath, window, wndWidth, wndHeight);
}

-(NSInteger) closeMedia
{
    mediaInfo = nil;
    return playerSDK->CloseMedia();
}

- (NSInteger) play
{
    return playerSDK->Play();
}

- (NSInteger) pause;
{
    return playerSDK->Pause();
}

- (NSInteger) seek:(NSUInteger)seekPos
{
    unsigned int pos = (unsigned int)seekPos;
    return playerSDK->Seek(pos);
}

- (NSInteger) getPlayingPos
{
    return playerSDK->GetPlayingPos();
}

- (NSInteger) getBufferingPos
{
    return playerSDK->GetBufferingPos();
}

- (EPlayerStatus) getPlayerStatus
{
    return (EPlayerStatus)playerSDK->GetPlayerStatus();
}

- (MediaInfo*) getMeidaInfo
{
    if(mediaInfo == nil)
    {
        if([self hasMediaActived])
        {
            const MediaContext *ctx = playerSDK->GetMeidaContext();
            if(ctx != NULL)
            {
                mediaInfo = [[MediaInfo alloc] init];
                mediaInfo.seekable = ctx->seekable;
                mediaInfo.hasAudio = ctx->hasAudio;
                mediaInfo.hasVideo = ctx->hasVideo;
                mediaInfo.duration = ctx->nDuration;
                mediaInfo.videoWidth = ctx->nVideoWidth;
                mediaInfo.videoHeight = ctx->nVideoHeight;
                mediaInfo.audioChannels = ctx->nChannels;
                mediaInfo.audioSampleRate = ctx->nSampleRate;
            }
        }
    }
    return mediaInfo;
}

- (BOOL) hasMediaActived
{
    PlayerStatus status = playerSDK->GetPlayerStatus();
    return (status == PlayerStatus_Opened ||
            status == PlayerStatus_Playing||
            status == PlayerStatus_Paused ||
            status == PlayerStatus_Seeking );
}

- (void) resizeVidoeWindow:(NSInteger)width
                    height:(NSInteger)height
{
    PlayerStatus status = playerSDK->GetPlayerStatus();
    if(status == PlayerStatus_Paused ||
       status == PlayerStatus_Playing||
       status == PlayerStatus_Seeking )
    {
        playerSDK->ResizeVideoWindow((unsigned int)width, (unsigned int)height);
    }
}

- (void) updateVideoWindow:(void*)window
               windowWidth:(NSUInteger)width
              windowHeight:(NSUInteger)height
{
    PlayerStatus status = playerSDK->GetPlayerStatus();
    if(status == PlayerStatus_Paused ||
       status == PlayerStatus_Playing||
       status == PlayerStatus_Seeking )
    {
        playerSDK->UpdateVideoWindow(window, (unsigned int)width, (unsigned int)height);
    }
}
@end

void NotifyHandler(void* pUserData, unsigned int msg)
{
    EPlayerAPI *playerAPI = (__bridge EPlayerAPI*)pUserData;
    if (playerAPI.msgHandler && [playerAPI.msgHandler respondsToSelector:@selector(handleEPlayerMsg:)])
    {
        [playerAPI.msgHandler handleEPlayerMsg:(EPlayerMessage)msg];
    }
}
