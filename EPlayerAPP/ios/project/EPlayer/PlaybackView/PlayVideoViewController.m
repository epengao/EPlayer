//
//  FullScreenViewController.m
//  Tapestry
//
//  Created by Kortide on 15/8/6.
//  Copyright (c) 2015年 Kortide. All rights reserved.
//

#import "EPlayerAPI.h"
#import "PlayVideoViewController.h"

#define TOP_bakButton_Tag         1001
#define TOP_bakLable_Tag          1002
#define TOP_fileName_Tag          1003
#define BOT_playPauseButton_Tag   2001
#define BOT_playTimeLable_Tag     2002
#define BOT_progressSlider_Tag    2003
#define BOT_durationTimeLable_Tag 2004
#define BOT_entryFullScreen_Tag   2005

#define TOP_View_Height    30
#define BOT_View_Height    50
#define BOT_Element_Height 30

@interface PlayVideoViewController () <EPlayerSdkDelegate>
{
    MediaInfo       *mediaInfo;
    EPlayerAPI      *eplayerAPI;
    EPlayerStatus   beforeScreenRotateStatus;
    EPlayerStatus   beforeEntryBackgroundStatus;

    NSUInteger      duration;
    UIView          *playControlView;
    UIView          *topMessageView;
    VideoWindow     *videoWindowView;

    UISlider        *playProgress;
    
    CGFloat         newWindowWidth;
    CGFloat         newWindowHeight;

    NSTimer         *updatePlayTimer;
    NSTimer         *dismissPlayCtrlViewTimer;
}
@end

@implementation PlayVideoViewController

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:UIDeviceOrientationDidChangeNotification object:nil];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    [self setupVideoImage];

    updatePlayTimer = nil;
    eplayerAPI = [EPlayerAPI sharedEPlayerAPI];
    eplayerAPI.msgHandler = self;
    beforeEntryBackgroundStatus = EPlayerStatus_Playing;
    NSInteger ret = [eplayerAPI openMediaPath:_videoFileURL
                                  videoWindow:videoWindowView
                                  windowWidth:videoWindowView.frame.size.width
                                 windowHeight:videoWindowView.frame.size.height];
    if (ret == EPlayer_Err_None)
    {
        mediaInfo = [eplayerAPI getMeidaInfo];
        duration = mediaInfo.duration;
    }
    [[NSNotificationCenter defaultCenter]addObserver:self selector:@selector(OrientationDidChange:) name:UIDeviceOrientationDidChangeNotification object:nil];
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    if(beforeEntryBackgroundStatus == EPlayerStatus_Playing)
        [eplayerAPI play];
    if(updatePlayTimer == nil)
    {
        updatePlayTimer = [NSTimer scheduledTimerWithTimeInterval:0.1 target:self selector:@selector(updatePlayTime) userInfo:nil repeats:YES];
    }
}

- (void)viewWillDisappear:(BOOL)animated
{
    if(updatePlayTimer != nil)
    {
        [updatePlayTimer invalidate];
        updatePlayTimer = nil;
    }
    beforeEntryBackgroundStatus = [eplayerAPI getPlayerStatus];
    [eplayerAPI pause];
    [super viewWillDisappear:animated];
}

- (void)setupVideoImage
{
    videoWindowView = [[VideoWindow alloc]initWithFrame:self.view.bounds];
    videoWindowView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    videoWindowView.contentMode = UIViewContentModeScaleAspectFit;
    videoWindowView.backgroundColor = [UIColor blackColor];
    videoWindowView.userInteractionEnabled = YES;
    UITapGestureRecognizer *pressed = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(videoWindowViewTouched:)];
    [videoWindowView addGestureRecognizer:pressed];
    [self.view addSubview:videoWindowView];
}

- (void)showTopMessageView
{
    float x = 0;
    float y = 0;
    float width = videoWindowView.bounds.size.width;
    if(topMessageView == nil)
    {
        topMessageView = [[UIView alloc]initWithFrame:CGRectMake(x, y, width, TOP_View_Height)];
        topMessageView.backgroundColor = [UIColor colorWithWhite:0.0 alpha:0.0];
 
        UIButton *backButton = [UIButton buttonWithType:UIButtonTypeCustom];
        backButton.tag = TOP_bakButton_Tag;
        [backButton setBackgroundColor:[UIColor clearColor]];
        [backButton setTitleColor:[UIColor colorWithWhite:1.0 alpha:1.0] forState:UIControlStateNormal];
        [backButton sizeToFit];
        [backButton setImage:[UIImage imageNamed:@"back"] forState:UIControlStateNormal];
        [backButton setFrame:CGRectMake(0, 0, backButton.frame.size.width, TOP_View_Height)];
        [backButton addTarget:self action:@selector(exitFullScreenButtonClicked:) forControlEvents:UIControlEventTouchUpInside];

        UILabel *fileNameLable = [[UILabel alloc]initWithFrame:CGRectMake(50, 0, topMessageView.bounds.size.width-100, TOP_View_Height)];
        fileNameLable.tag = TOP_fileName_Tag;
        fileNameLable.text = [self.videoFileURL lastPathComponent];
        fileNameLable.textAlignment = NSTextAlignmentCenter;
        [fileNameLable setLineBreakMode:NSLineBreakByTruncatingMiddle];
        [fileNameLable setTextColor:[UIColor whiteColor]];
        [fileNameLable setBackgroundColor:[UIColor clearColor]];
        [fileNameLable setTextAlignment:NSTextAlignmentCenter];
        
        UIColor *colorOne = [UIColor colorWithRed:0 green:0 blue:0 alpha:0.6];
        UIColor *colorTwo = [UIColor colorWithRed:0 green:0 blue:0 alpha:0.0];
        NSArray *colors = [NSArray arrayWithObjects:(id)colorOne.CGColor, colorTwo.CGColor, nil];
        NSNumber *stopOne = [NSNumber numberWithFloat:0.0];
        NSNumber *stopTwo = [NSNumber numberWithFloat:1.0];
        NSArray *locations = [NSArray arrayWithObjects:stopOne, stopTwo, nil];
        CAGradientLayer *colorLayer = [CAGradientLayer layer];
        colorLayer.colors = colors;
        colorLayer.locations = locations;
        colorLayer.frame = topMessageView.bounds;

        [topMessageView addSubview:fileNameLable];
        [topMessageView addSubview:backButton];
        [topMessageView.layer insertSublayer:colorLayer atIndex:0];

        [self.view insertSubview:topMessageView aboveSubview:videoWindowView];
    }
    else
    {
        [topMessageView setFrame:CGRectMake(x, y, width, TOP_View_Height)];
        if(topMessageView.isHidden)
        {
            [topMessageView setHidden:NO];
        }
        else
        {
            [topMessageView setHidden:YES];
        }
    }
}

- (void)showPlayControlView
{
    if(playControlView == nil)
    {
        float x = 0;
        float y = self.view.frame.size.height - BOT_View_Height;
        float bottomViewHeight = BOT_View_Height;
        float bottomViewWidth = videoWindowView.bounds.size.width;

        CGRect bottomViewFrame = CGRectMake(x, y, bottomViewWidth, bottomViewHeight);
        playControlView = [[UIView alloc]initWithFrame:bottomViewFrame];
        playControlView.backgroundColor = [UIColor colorWithWhite:0.0 alpha:0.0];

        CGFloat buttonWidth = BOT_Element_Height*1.5;
        CGFloat buttonHeight = BOT_Element_Height*1.5;
        CGFloat element_Y = (BOT_View_Height - buttonHeight) * 0.5;
        UIButton *playPauseButton = [[UIButton alloc]initWithFrame:CGRectMake(0, element_Y, buttonWidth, buttonHeight)];
        [playPauseButton setTag:BOT_playPauseButton_Tag];
        [playPauseButton setBackgroundColor:[UIColor clearColor]];
        [playPauseButton setImage:[UIImage imageNamed:@"pause"] forState:UIControlStateNormal];
        [playPauseButton addTarget:self action:@selector(playPauseButtonClicked:) forControlEvents:UIControlEventTouchUpInside];
        [playControlView addSubview:playPauseButton];

        element_Y = (BOT_View_Height - buttonHeight*0.4) * 0.5;
        UIButton *fullScreenButton = [[UIButton alloc]initWithFrame:CGRectMake(bottomViewWidth-buttonWidth, element_Y, buttonWidth*0.4, buttonHeight*0.4)];
        [fullScreenButton setTag:BOT_entryFullScreen_Tag];
        [fullScreenButton setBackgroundColor:[UIColor clearColor]];
        [fullScreenButton setImage:[UIImage imageNamed:@"entryFullScreen"] forState:UIControlStateNormal];
        [fullScreenButton addTarget:self action:@selector(entryExitFullScreenClick:) forControlEvents:UIControlEventTouchUpInside];
        [playControlView addSubview:fullScreenButton];

        CGFloat lableWidth = BOT_Element_Height * 1.5;
        NSString *playTimeStr = [self getTimeTitle:0];
        element_Y = (BOT_View_Height - BOT_Element_Height) * 0.5;
        CGRect playTimeViewFrame = CGRectMake(buttonWidth, element_Y, lableWidth, BOT_Element_Height);
        UILabel *playTimeLableView = [[UILabel alloc]initWithFrame:playTimeViewFrame];
        playTimeLableView.textAlignment = NSTextAlignmentCenter;
        [playTimeLableView setTag:BOT_playTimeLable_Tag];
        [playTimeLableView setBackgroundColor:[UIColor clearColor]];
        [playTimeLableView setText:playTimeStr];
        [playTimeLableView setFont:[UIFont systemFontOfSize:10]];
        [playTimeLableView setTextColor:[UIColor colorWithWhite:1.0 alpha:1.0]];
        [playControlView addSubview:playTimeLableView];

        NSString *durationTimeStr = [self getTimeTitle:mediaInfo.duration];
        CGRect durationTimeViewFrame = CGRectMake(bottomViewWidth-buttonWidth-lableWidth, element_Y, lableWidth, BOT_Element_Height);
        UILabel *durationTimeLableView = [[UILabel alloc]initWithFrame:durationTimeViewFrame];
        durationTimeLableView.textAlignment = NSTextAlignmentCenter;
        [durationTimeLableView setTag:BOT_durationTimeLable_Tag];
        [durationTimeLableView setBackgroundColor:[UIColor clearColor]];
        [durationTimeLableView setText:durationTimeStr];
        [durationTimeLableView setFont:[UIFont systemFontOfSize:10]];
        [durationTimeLableView setTextColor:[UIColor colorWithWhite:1.0 alpha:1.0]];
        CGPoint center1 = durationTimeLableView.center;
        center1.y = playControlView.frame.size.height / 2;
        [durationTimeLableView setCenter:center1];
        [playControlView addSubview:durationTimeLableView];

        /* playback progress */
        CGFloat progressBarWidth = bottomViewWidth - buttonWidth*2 - lableWidth*2;
        playProgress = [[UISlider alloc]initWithFrame:CGRectMake((buttonWidth+lableWidth), element_Y, progressBarWidth, BOT_Element_Height)];
        [playProgress setTag:BOT_playPauseButton_Tag];
        [playProgress setMinimumValue:0];
        [playProgress setMaximumValue:mediaInfo.duration];
        [playProgress setContinuous:YES];
        [playProgress setThumbImage:[UIImage imageNamed:@"progress_dot"] forState:UIControlStateNormal];
        [playProgress addTarget:self action:@selector(onPlaybackProgressChange:) forControlEvents:UIControlEventValueChanged];
        [playControlView addSubview:playProgress];
        
        UIColor *colorOne = [UIColor colorWithRed:0 green:0 blue:0 alpha:0.0];
        UIColor *colorTwo = [UIColor colorWithRed:0 green:0 blue:0 alpha:0.6];
        NSArray *colors = [NSArray arrayWithObjects:(id)colorOne.CGColor, colorTwo.CGColor, nil];
        NSNumber *stopOne = [NSNumber numberWithFloat:1.0];
        NSNumber *stopTwo = [NSNumber numberWithFloat:0.0];
        NSArray *locations = [NSArray arrayWithObjects:stopTwo, stopOne, nil];
        CAGradientLayer *colorLayer = [CAGradientLayer layer];
        colorLayer.colors = colors;
        colorLayer.locations = locations;
        colorLayer.frame = playControlView.bounds;
        [playControlView.layer insertSublayer:colorLayer atIndex:0];

        [self.view insertSubview:playControlView aboveSubview:videoWindowView];
    }
    else
    {
        if(playControlView.isHidden)
        {
            [playControlView setHidden:NO];
            [playProgress setHidden:NO];
        }
        else
        {
            [playControlView setHidden:YES];
            [playProgress setHidden:YES];
        }
    }
}

- (BOOL)shouldAutorotate
{
    return YES;
}

- (UIInterfaceOrientationMask)supportedInterfaceOrientations
{
    return UIInterfaceOrientationMaskAllButUpsideDown;
}

#pragma mark - UI Widget Event

- (void)videoWindowViewTouched :(id)sender
{
    [self showTopMessageView];
    [self showPlayControlView];
    if(dismissPlayCtrlViewTimer != nil)
    {
        [dismissPlayCtrlViewTimer invalidate];
        dismissPlayCtrlViewTimer = nil;

    }
    dismissPlayCtrlViewTimer = [NSTimer scheduledTimerWithTimeInterval:2 target:self selector:@selector(dismissPlayCtrlView) userInfo:nil repeats:NO];
}

- (void)dismissPlayCtrlView
{
    [topMessageView setHidden:YES];
    [playControlView setHidden:YES];
    if(dismissPlayCtrlViewTimer == nil)
    {
        [dismissPlayCtrlViewTimer invalidate];
        dismissPlayCtrlViewTimer = nil;
    }
}

- (void)play
{
    [eplayerAPI play];
}

- (void)pause
{
    [eplayerAPI pause];
}

- (void)playPauseButtonClicked :(id)sender
{
    EPlayerStatus status;
    status = [eplayerAPI getPlayerStatus];
    if (status == EPlayerStatus_Playing)
    {
        [self pause];
        [sender setImage:[UIImage imageNamed:@"play"] forState:UIControlStateNormal];
    }
    else
    {
        [self play];
        [sender setImage:[UIImage imageNamed:@"pause"] forState:UIControlStateNormal];
    }
}

- (void)entryExitFullScreenClick :(id)sender
{
}

-(void)onSeek :(UISlider*)sender
{
    int playPos = (int)sender.value;
    [eplayerAPI seek:playPos];
}

-(void)onPlaybackProgressChange :(UISlider*)sender
{
    [eplayerAPI seek:sender.value];
}

- (void)exitFullScreenButtonClicked :(id)sender
{
    [eplayerAPI closeMedia];
    [self dismissViewControllerAnimated:YES completion:^{}];
}

#pragma mark - private methods
- (void)updatePlayTime
{
    NSUInteger playTime = [eplayerAPI getPlayingPos];
    NSString *playTimeStr = [self getTimeTitle:playTime];
    UILabel *timeLabel = [playControlView viewWithTag:BOT_playTimeLable_Tag];
    [timeLabel setText:playTimeStr];
    
    [playProgress setValue:playTime];
}

- (NSString*)getTimeTitle :(NSUInteger)time
{
    NSUInteger s = (int)time/1000;
    NSUInteger m = s/60;
    NSUInteger h = m/60;

    NSUInteger hh = h;
    NSUInteger mm = m%60;
    NSUInteger ss = s%60;

    NSString *hhStr = nil;
    NSString *mmStr = nil;
    NSString *ssStr = nil;

    if(hh <= 0)
    {
        hhStr = @"";
        mmStr = [NSString stringWithFormat:@"%lu:", mm];
        ssStr = [NSString stringWithFormat:@"%02lu", ss];
    }
    else
    {
        hhStr = [NSString stringWithFormat:@"%lu:", hh];
        mmStr = [NSString stringWithFormat:@"%02lu:", mm];
        ssStr = [NSString stringWithFormat:@"%02lu", ss];
    }

    return [NSString stringWithFormat:@"%@%@%@", hhStr, mmStr, ssStr];
}

#pragma mark - EPlayerSDK delegate
-(void)handleEPlayerMsg:(EPlayerMessage)msg
{
    if(msg == EPlayer_Msg_PlayStop)
    {
        dispatch_sync(dispatch_get_main_queue(),^
        {
            [eplayerAPI closeMedia];
        });
    }
}

#pragma mark - View rotation
- (void)updateVideoWindowFrame
{
    if(beforeScreenRotateStatus == EPlayerStatus_Playing)
    {
        [eplayerAPI play];
    }
    [eplayerAPI updateVideoWindow:videoWindowView
                      windowWidth:self.view.frame.size.width
                     windowHeight:self.view.frame.size.height];
    [videoWindowView layoutSubviews];

    [self updateUI];
}

//- (void) viewWillTransitionToSize:(CGSize)size withTransitionCoordinator:(id )coordinator
//{
//    newWindowWidth = size.width;
//    newWindowHeight = size.height;
//
//}
- (void)viewWillTransitionToSize:(CGSize)size withTransitionCoordinator:(id )coordinator
{
    newWindowWidth = size.width;
    newWindowHeight = size.height;
    beforeScreenRotateStatus = [eplayerAPI getPlayerStatus];
    [eplayerAPI pause];
}

-(void)OrientationDidChange:(NSNotification*)notification
{
    UIDeviceOrientation Orientation=[[UIDevice currentDevice]orientation];
    if(Orientation==UIDeviceOrientationLandscapeLeft || Orientation==UIDeviceOrientationLandscapeRight)
    {
        NSLog(@"Landscape");
    }
    else if(Orientation==UIDeviceOrientationPortrait)
    {
        NSLog(@"Potrait Mode");
    }
    [self performSelector:@selector(updateVideoWindowFrame) withObject:nil afterDelay:0.02];
}

#pragma mark - Screen top bar status
- (BOOL)prefersStatusBarHidden
{
    return YES;
}

#pragma mark - Update UI after Rotation
- (void)updateUI
{
    [self updateToViewMessageView];
    [self updateBottomMediaCtrlView];
}
-(void)updateToViewMessageView
{
    CGFloat x = 0;
    CGFloat y = 0;
    CGFloat height = TOP_View_Height;
    CGFloat width = videoWindowView.bounds.size.width;
    CGRect topViewFrame = CGRectMake(x, y, width, height);

    CGFloat lableWidth = width - 100;
    CGRect lableFrame = CGRectMake(50, 0, lableWidth, height);
    UILabel *fileNameLable = [topMessageView viewWithTag:TOP_fileName_Tag];
    [fileNameLable setFrame:lableFrame];
    
    CAGradientLayer *colorLayer = (CAGradientLayer*)topMessageView.layer.sublayers[0];
    [colorLayer setFrame:topViewFrame];

    [topMessageView setFrame:topViewFrame];
}
-(void)updateBottomMediaCtrlView
{
    float x = 0;
    float y = self.view.frame.size.height - BOT_View_Height;
    float bottomViewHeight = BOT_View_Height;
    float bottomViewWidth = videoWindowView.bounds.size.width;
    CGRect bottomViewFrame = CGRectMake(x, y, bottomViewWidth, bottomViewHeight);

    CGFloat buttonWidth = BOT_Element_Height*1.5;
    CGFloat buttonHeight = BOT_Element_Height*1.5;
    UIButton *playPauseButton = [playControlView viewWithTag:BOT_playPauseButton_Tag];
    CGFloat element_Y = (BOT_View_Height - buttonHeight) * 0.5;
    [playPauseButton setFrame :CGRectMake(0, element_Y, buttonWidth, buttonHeight)];

    UIButton *fullScreenButton = [playControlView viewWithTag:BOT_entryFullScreen_Tag];
    element_Y = (BOT_View_Height - buttonHeight*0.4) * 0.5;
    [fullScreenButton setFrame:CGRectMake(bottomViewWidth-buttonWidth, element_Y, buttonWidth*0.4, buttonHeight*0.4)];
    
    CGFloat lableWidth = BOT_Element_Height * 1.5;
    element_Y = (BOT_View_Height - BOT_Element_Height) * 0.5;
    UILabel *playTimeLableView = [playControlView viewWithTag:BOT_playTimeLable_Tag];
    CGRect playTimeViewFrame = CGRectMake(buttonWidth, element_Y, lableWidth, BOT_Element_Height);
    [playTimeLableView setFrame:playTimeViewFrame];

    UILabel *durationTimeLableView = [playControlView viewWithTag:BOT_durationTimeLable_Tag];
    CGRect durationTimeViewFrame = CGRectMake(bottomViewWidth-buttonWidth-lableWidth, element_Y, lableWidth, BOT_Element_Height);
    [durationTimeLableView setFrame:durationTimeViewFrame];

    /* playback progress */
    CGFloat progressBarWidth = bottomViewWidth - buttonWidth*2 - lableWidth*2;
    [playProgress setFrame:CGRectMake((buttonWidth+lableWidth), element_Y, progressBarWidth, BOT_Element_Height)];

    [playControlView setFrame:bottomViewFrame];
    CAGradientLayer *colorLayer = (CAGradientLayer*)playControlView.layer.sublayers[0];
    [colorLayer setFrame:CGRectMake(0, 0, playControlView.bounds.size.width, playControlView.bounds.size.height)];
}
@end
