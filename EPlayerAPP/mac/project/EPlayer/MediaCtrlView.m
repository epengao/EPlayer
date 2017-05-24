//
//  MediaCtrlView.m
//  EPlayer
//
//  Created by GaoPeng on 2017/5/14.
//  Copyright © 2017年 EC. All rights reserved.
//

#import "LADSlider.h"
#import "EPlayerAPI.h"
#import "MediaCtrlView.h"
#import "ISSoundAdditions.h"
#import "MediaCtrlViewShadow.h"

/* Media Ctrl */

@interface MediaCtrlView ()
@property BOOL needUpdatePlaytime;
@property MediaCtrlViewShadow* shadowView;
@property NSTimer* progressBarTimer;
@property (weak) IBOutlet LADSlider *progressBar;
@property (weak) IBOutlet NSButton *playStopButton;
@property (weak) IBOutlet NSButton *playPauseButton;
@property (weak) IBOutlet NSButton *volumeCtrlButton;
@property (weak) IBOutlet NSButton *fullScreenButton;
@property (weak) IBOutlet LADSlider *volumeCtrlSlider;
@property (weak) IBOutlet NSTextField *playTimeDurationTextLable;
@end

@implementation MediaCtrlView
- (id)init
{
    self = [super init];
    if(self)
    {
        self.mainCtrl = nil;
        self.playerSDK = nil;
        self.needUpdatePlaytime = NO;
    }
    return self;
}

- (void)addBackgroundMat
{
}

- (void)drawRect:(NSRect)dirtyRect
{
    [super drawRect:dirtyRect];
    
    NSRect renderRect = NSMakeRect(0, 0, dirtyRect.size.width, dirtyRect.size.height);

    NSRect topHalf, bottomHalf;
    NSDivideRect(renderRect, &topHalf, &bottomHalf, floor(renderRect.size.height), NSMaxYEdge);
    
    NSBezierPath * path = [NSBezierPath bezierPathWithRoundedRect:self.bounds xRadius:4.0 yRadius:4.0];
    [[NSBezierPath bezierPathWithRect:renderRect] addClip];
    
    NSGradient * gradient1 = [[NSGradient alloc] initWithStartingColor:[NSColor colorWithCalibratedWhite:0.10 alpha:1] endingColor:[NSColor colorWithCalibratedWhite:0.10 alpha:1]];
    //NSGradient * gradient2 = [[NSGradient alloc] initWithStartingColor:[NSColor colorWithCalibratedWhite:0.2 alpha:1] endingColor:[NSColor colorWithCalibratedWhite:0.05 alpha:1]];
    [path addClip];

    [gradient1 drawInRect:topHalf angle:90.0];
    //[gradient2 drawInRect:bottomHalf angle:270.0];
    [[NSColor blackColor] set];
    NSRectFill(NSMakeRect(0, 5, self.bounds.size.width, 1.0));
}

- (void)initVolumeCtrl
{
    _volumeCtrlSlider.minValue = 0.0;
    _volumeCtrlSlider.maxValue = 1.0;
    _volumeCtrlSlider.doubleValue = 0.5;
    [_volumeCtrlSlider setTarget:self];
    [_volumeCtrlSlider setAction:@selector(volumeSliderAction:)];
    [_volumeCtrlButton setState:NSOnState];
    [_volumeCtrlButton setImage:[NSImage imageNamed:@"volume"]];
    [_volumeCtrlSlider setKnobImage:[NSImage imageNamed:@"volume_knob"]];
    [_volumeCtrlSlider setMinimumValueImage:[NSImage imageNamed:@"volume_left"]];
    [_volumeCtrlSlider setMaximumValueImage:[NSImage imageNamed:@"volume_right"]];
}

- (void)initFullScreenButton
{
    [_fullScreenButton setImage:[NSImage imageNamed:@"entry_full"]];
}

- (void)initPlayStopButton
{
    [_playStopButton setImage:[NSImage imageNamed:@"stop"]];
}

-(void)initShadowView
{
    CGFloat width = 240;
    CGFloat height = 40;
    CGFloat x = (self.frame.size.width - width) * 0.5;
    CGFloat y = (self.frame.size.height - height) * 0.5 - 3;
    _shadowView = [[MediaCtrlViewShadow alloc] initWithFrame:NSMakeRect(x, y, width, height)];
    [self addSubview:_shadowView positioned:NSWindowBelow relativeTo:nil];
}

- (void)initPlayPauseButton
{
    [_playPauseButton setImage:[NSImage imageNamed:@"play"]];
}

- (void)initProgressBar
{
    NSImage *sliderKnob = [NSImage imageNamed:@"progressbar_knob"];
    NSImage *sliderLeftImage = [NSImage imageNamed:@"progressbar_on_background"];
    NSImage *sliderRightImage = [NSImage imageNamed:@"progressbar_off_background"];

    [_progressBar setKnobImage:sliderKnob];
    [_progressBar setMinimumValueImage:sliderLeftImage];
    [_progressBar setMaximumValueImage:sliderRightImage];

    _progressBar.minValue = 0.0;
    _progressBar.maxValue = 100;
    _progressBar.floatValue = 0.0;

    [_progressBar setTarget:self];
    [_progressBar setAction:@selector(progressBarAction:)];

    [_playTimeDurationTextLable setHidden:YES];
}

- (void)updateViewFrame:(NSRect)rect
{
    if(rect.size.width * 0.5 < 240 * 0.5 + 120 + 20)
    {
        [_playTimeDurationTextLable setHidden:YES];
    }
    else
    {
        if([_playTimeDurationTextLable isHidden] && [_playerSDK hasMediaActived])
        {
            [_playTimeDurationTextLable setHidden:NO];
        }
    }
    [self setFrame:rect];

    CGFloat width = 240;
    CGFloat height = 40;
    CGFloat x = (rect.size.width - width) * 0.5;
    CGFloat y = (rect.size.height - height) * 0.5 - 3;
    [_shadowView setFrame:NSMakeRect(x, y, width, height)];
}

#pragma mark - progress bar control
- (void)updateProgressBarValue
{
    float value = 0.0;
    if(_playerSDK != nil && [_playerSDK hasMediaActived])
    {
        value = [_playerSDK getPlayingPos];
    }
    _progressBar.doubleValue = value;
    [_progressBar setNeedsDisplay];

    NSString* playTimeString = [self getTimeString:value];
    NSString* oldPlayTimeDurationString = _playTimeDurationTextLable.stringValue;
    NSString* playTimeDurationString = [NSString stringWithFormat:@"%@ / %@", playTimeString, [oldPlayTimeDurationString substringFromIndex:11]];
    [_playTimeDurationTextLable setStringValue:playTimeDurationString];
}
- (IBAction)progressBarAction:(LADSlider *)sender
{
    if(_playerSDK != nil && [_playerSDK hasMediaActived])
    {
        [_playerSDK seek:sender.doubleValue];
    }
    else
    {
        [self performSelector:@selector(updateProgressBarValue) withObject:nil afterDelay:0.01];
    }
}
- (void)updateProgressBarInfo:(float)min
                          max:(float)max
                    currValue:(float)value
                       runing:(BOOL)run
{
    _progressBar.minValue = min;
    _progressBar.maxValue = max;
    _progressBar.doubleValue = value;
    NSString *startTime = [self getTimeString:min];
    NSString *durationTime = [self getTimeString:max];
    if(run)
    {
        //_progressBarTimer = [NSTimer scheduledTimerWithTimeInterval:0.05 target:self selector:@selector(updateProgressBarValue) userInfo:nil repeats:YES];
// 通过另外起一个线程来轮询更新进度条，在这个线程里在切到主线程更新UI。
        _needUpdatePlaytime = YES;
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
            while (_needUpdatePlaytime)
            {
                [NSThread sleepForTimeInterval:0.05];
                dispatch_async(dispatch_get_main_queue(), ^{
                    [self updateProgressBarValue];
                });
            }
        });
        _playTimeDurationTextLable.stringValue = [NSString stringWithFormat:@"%@ / %@", startTime, durationTime];
        [_playTimeDurationTextLable setHidden:NO];
    }
    else
    {
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
            [NSThread sleepForTimeInterval:0.25];
            _needUpdatePlaytime = NO;
            _progressBar.doubleValue = 0.0;
            [_progressBar setNeedsDisplay:YES];
        });
        _playTimeDurationTextLable.stringValue = @"00:00:00 / 00:00:00";
        [_playTimeDurationTextLable setHidden:YES];
    }
}

#pragma mark - play/pause button control
-(void)updatePlayPauseUI:(BOOL)canBePaused
{
    if(canBePaused)
    {
        [_playPauseButton setImage:[NSImage imageNamed:@"pause"]];
    }
    else
    {
        [_playPauseButton setImage:[NSImage imageNamed:@"play"]];
    }
}
- (IBAction)playPause:(NSButton *)sender
{
    if(_playerSDK != nil)
    {
        EPlayerStatus status = [_playerSDK getPlayerStatus];
        if(status == EPlayerStatus_Playing)
        {
            [_playerSDK pause];
            [self updatePlayPauseUI:NO];
        }
        else
        {
            if([_playerSDK play] == EPlayer_Err_None)
            {
                [self updatePlayPauseUI:YES];
            }
        }
    }
}

#pragma mark - volume control
-(void)updateVolumeUI:(BOOL)mute
{
    if(mute)
    {
        [_volumeCtrlButton setImage:[NSImage imageNamed:@"volume_mute"]];
    }
    else
    {
        [_volumeCtrlButton setImage:[NSImage imageNamed:@"volume"]];
    }
}
- (IBAction)volumeButtonClicked:(NSButton *)sender
{
    if(sender.state == NSOffState)
    {
        [NSSound applyMute:YES];
        [self updateVolumeUI:YES];
    }
    else
    {
        [NSSound applyMute:NO];
        [self updateVolumeUI:NO];
    }
}
- (IBAction)volumeSliderAction:(LADSlider *)sender
{
    [self updateVolumeUI:NO];
    [_volumeCtrlButton setState:NSOnState];
    [NSSound setSystemVolume:sender.doubleValue];
}

#pragma mark - stop button control
- (IBAction)stopButtonClicked:(NSButton *)sender
{
    if(_playerSDK != nil && [_playerSDK hasMediaActived])
    {
        [_playerSDK closeMedia];
    }
    [self.mainCtrl mediaStopedNotify];
}

#pragma mark - full screen button control
- (IBAction)fullScreenButtonClicked:(NSButton *)sender
{
    [self.mainCtrl entryFullScreen];
}

-(NSString*)getTimeString:(int)time
{
    int toal_s = time / 1000;
    int hh = toal_s / 3600;
    int mm = (toal_s % 3600) / 60;
    int ss = toal_s % 60;

    return [NSString stringWithFormat:@"%02d:%02d:%02d",hh, mm, ss];
}
@end
