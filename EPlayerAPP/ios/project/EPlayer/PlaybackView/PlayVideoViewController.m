//
//  FullScreenViewController.m
//  Tapestry
//
//  Created by Kortide on 15/8/6.
//  Copyright (c) 2015年 Kortide. All rights reserved.
//

#import "EPlayerAPI.h"
#import "PlayVideoViewController.h"

@interface PlayVideoViewController () <EPlayerSdkDelegate>
{
    MediaInfo       *mediaInfo;
    EPlayerAPI      *eplayerAPI;
    EPlayerStatus   beforeEntryBackgroundStatus;

    NSUInteger      duration;
    UIView          *playControlView;
    UIView          *topMessageView;
    VideoWindow     *videoWindowView;

    NSDate          *startTime;
    NSDateFormatter *timeFormatter;
    
    UISlider        *playProgress;
}
@end

@implementation PlayVideoViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [self setupVideoImage];

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
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    if(beforeEntryBackgroundStatus == EPlayerStatus_Playing)
        [eplayerAPI play];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
    beforeEntryBackgroundStatus = [eplayerAPI getPlayerStatus];
    [eplayerAPI pause];
}

- (void)setupVideoImage
{
    videoWindowView = [[VideoWindow alloc]initWithFrame:self.view.bounds];
    videoWindowView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    videoWindowView.backgroundColor = [UIColor blackColor];
    videoWindowView.userInteractionEnabled = YES;
    UITapGestureRecognizer *pressed = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(videoImageClicked:)];
    [videoWindowView addGestureRecognizer:pressed];
    [self.view addSubview:videoWindowView];
}

- (void)showTopMessageView
{
    if(topMessageView == nil)
    {
        float hight = 50.0;
        float width = videoWindowView.bounds.size.width;
        float x = 0;
        float y = 0;
        topMessageView = [[UIView alloc]initWithFrame:CGRectMake(x, y, width, hight)];
        topMessageView.backgroundColor = [UIColor colorWithWhite:0.0 alpha:0.6];
 
        UIButton *backButton = [UIButton buttonWithType:UIButtonTypeCustom];
        [backButton setBackgroundColor:[UIColor clearColor]];
        [backButton setTitleColor:[UIColor colorWithWhite:1.0 alpha:0.6] forState:UIControlStateNormal];
        [backButton.titleLabel setFont:[UIFont systemFontOfSize:16]];
        [backButton sizeToFit];
        [backButton setImage:[UIImage imageNamed:@"back"] forState:UIControlStateNormal];
        [backButton setFrame:CGRectMake(0, 0, backButton.frame.size.width, hight)];
        [backButton addTarget:self action:@selector(exitFullScreenButtonClicked:) forControlEvents:UIControlEventTouchUpInside];
        
        UILabel *fileNameLable = [[UILabel alloc]initWithFrame:CGRectMake(0, 0, topMessageView.bounds.size.width, hight)];
        fileNameLable.text = [self.videoFileURL lastPathComponent];
        [fileNameLable setTextColor:[UIColor whiteColor]];
        [fileNameLable setBackgroundColor:[UIColor clearColor]];
        [fileNameLable setTextAlignment:NSTextAlignmentCenter];
        
        [topMessageView addSubview:fileNameLable];
        [topMessageView addSubview:backButton];
        [self.view insertSubview:topMessageView aboveSubview:videoWindowView];
    }
    else
    {
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
        float hight = 45.0;
        float width = videoWindowView.bounds.size.width;
        float x = 0;
        float y = videoWindowView.bounds.size.height - hight;
        playControlView = [[UIView alloc]initWithFrame:CGRectMake(x, y, width, hight)];
        playControlView.backgroundColor = [UIColor colorWithWhite:0.0 alpha:0.6];
        
        UIButton *pauseButton = [[UIButton alloc]initWithFrame:CGRectMake(20.0+23.0/2-hight/2, 0, hight, hight)];
        [pauseButton setBackgroundColor:[UIColor clearColor]];
        [pauseButton setImage:[UIImage imageNamed:@"pause"] forState:UIControlStateNormal];
        [pauseButton addTarget:self action:@selector(pauseButtonClicked:) forControlEvents:UIControlEventTouchUpInside];
        [playControlView addSubview:pauseButton];
    
        /* duration label */
        NSString *currTime = [self getTimeTitle:0];
        NSString *totalTime = [self getTimeTitle:duration];
        NSString *showTime = [NSString stringWithFormat:@"%@ / %@",currTime,totalTime];
        UILabel *playbakTime = [[UILabel alloc]init];
        [playbakTime setTag:1001];
        [playbakTime setBackgroundColor:[UIColor clearColor]];
        [playbakTime setText:showTime];
        [playbakTime setTextColor:[UIColor colorWithWhite:1.0 alpha:0.6]];
        [playbakTime setFont:[UIFont systemFontOfSize:16]];
        [playbakTime setFrame:CGRectMake(pauseButton.frame.origin.x+pauseButton.frame.size.width + 20, 0, 200, hight)];
        [playControlView addSubview:playbakTime];

        /* playback progress */
        playProgress = [[UISlider alloc]initWithFrame:CGRectMake(0, y-10, width, 21)];
        [playProgress setMinimumValue:0];
        [playProgress setMaximumValue:duration];
        [playProgress setContinuous:YES];
        [playProgress setThumbImage:[UIImage imageNamed:@"progress_dot"] forState:UIControlStateNormal];
        [playProgress addTarget:self action:@selector(onPlaybackProgressChange:) forControlEvents:UIControlEventValueChanged];
        [self.view addSubview:playProgress];

        UIButton *exitFullScreenButton = [[UIButton alloc]initWithFrame:CGRectMake(width-20-24/2-hight/2, 0, hight, hight)];
        [exitFullScreenButton setBackgroundColor:[UIColor clearColor]];
        [exitFullScreenButton setImage:[UIImage imageNamed:@"exit"] forState:UIControlStateNormal];
        [exitFullScreenButton addTarget:self action:@selector(exitFullScreenButtonClicked:) forControlEvents:UIControlEventTouchUpInside];
        exitFullScreenButton.tag = 6;
        [playControlView addSubview:exitFullScreenButton];

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
    return NO;
}

- (UIInterfaceOrientationMask)supportedInterfaceOrientations
{
    return UIInterfaceOrientationMaskLandscapeRight;
}

- (UIInterfaceOrientation)preferredInterfaceOrientationForPresentation
{
    return UIInterfaceOrientationLandscapeRight;
}

#pragma mark - UI Widget Event

- (void)videoImageClicked :(id)sender
{
    [self showTopMessageView];
    [self showPlayControlView];
}

- (void)hideControlView
{
    [topMessageView setHidden:YES];
    [playControlView setHidden:YES];
    [playProgress setHidden:YES];
}

- (void)play
{
    [eplayerAPI play];
}

- (void)pause
{
    [eplayerAPI pause];
}

- (void)pauseButtonClicked :(id)sender
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

-(void)onSeek :(UISlider*)sender
{
    int playPos = (int)sender.value;
    [eplayerAPI seek:playPos];
}

-(void)onPlaybackProgressChange :(UISlider*)sender
{
    [eplayerAPI seek:sender.value];
    [self updatePlayTimeLable:(int)sender.value];
}

- (void)exitFullScreenButtonClicked :(id)sender
{
    [eplayerAPI closeMedia];
    [self dismissViewControllerAnimated:YES completion:^{}];
}

#pragma mark - private methods
- (void)updatePlayTimeLable:(unsigned int)time
{
    NSString *currTime = [self getTimeTitle:time];
    NSString *totalTime = [self getTimeTitle:duration];
    NSString *showTime = [NSString stringWithFormat:@"%@ / %@",currTime,totalTime];
    UILabel *timeLabel = (UILabel*)[self.view viewWithTag:1001];
    [timeLabel setText:showTime];
}

- (NSString*)getTimeTitle :(NSUInteger)time
{
    
    int s = (int)time/1000;
    int m = s/60;
    int h = m/60;

    int hh = h;
    int mm = m%60;
    int ss = s%60;

    NSString *hhStr = nil;
    NSString *mmStr = nil;
    NSString *ssStr = nil;

    if(hh < 10)
        hhStr = [NSString stringWithFormat:@"0%d", hh];
    else
        hhStr = [NSString stringWithFormat:@"%d", hh];
    if(mm < 10)
        mmStr = [NSString stringWithFormat:@"0%d", mm];
    else
        mmStr = [NSString stringWithFormat:@"%d", mm];
    if(ss < 10)
        ssStr = [NSString stringWithFormat:@"0%d", ss];
    else
        ssStr = [NSString stringWithFormat:@"%d", ss];

    return [NSString stringWithFormat:@"%@:%@:%@", hhStr, mmStr, ssStr];
}

#pragma mark - MediaEngine delegate
- (void)playbackTimeUpdate:(unsigned int)currPlayTime
{
    [playProgress setValue:currPlayTime animated:YES];
    [self updatePlayTimeLable:currPlayTime];
}

- (int)mediaStoped
{
    NSLog(@"MediaEngine stoped.");
    return 0;
}

- (int)playbackFinished
{
    NSLog(@"MeidaEngine playback finished.");
    dispatch_async(dispatch_get_main_queue(), ^
    {
        [eplayerAPI closeMedia];
        [self dismissViewControllerAnimated:YES completion:^{}];
    });
    return 0;
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

@end
