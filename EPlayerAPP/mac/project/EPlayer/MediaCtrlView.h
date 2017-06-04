//
//  WinTitleView.h
//  EPlayer
//
//  Created by GaoPeng on 2017/5/14.
//  Copyright © 2017年 EC. All rights reserved.
//

#import "EPlayerAPI.h"
#import <Cocoa/Cocoa.h>
#import "AppDelegate.h"

@interface MediaCtrlView : NSView
@property MediaInfo* mediaInfo;
@property AppDelegate* mainCtrl;
@property EPlayerAPI* playerSDK;
- (void)initShadowView;
- (void)initVolumeCtrl;
- (void)initProgressBar;
- (void)initPlayStopButton;
- (void)initPlayPauseButton;
- (void)initFullScreenButton;
- (void)updateVolumeUI:(BOOL)mute;
- (void)updatePlayPauseUI:(BOOL)canBePaused;
- (void)updateViewFrame:(NSRect)rect;
- (void)updateProgressBarInfo:(float)min max:(float)max currValue:(float)value runing:(BOOL)run;
@end
