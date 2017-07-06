//
//  ViewController.m
//  demoPlayer
//
//  Created by Anthon Liu on 2017/7/6.
//  Copyright © 2017年 EC. All rights reserved.
//

#import "EPlayerAPI.h"
#import "ViewController.h"

@interface ViewController ()
{
    BOOL loadMedia;
    EPlayerAPI* playerAPI;
    VideoWindow* videoWindow;
}
@property (weak, nonatomic) IBOutlet UIButton *playPause;
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    loadMedia = NO;
    playerAPI = [EPlayerAPI sharedEPlayerAPI];
    CGFloat videoWindowWidth = self.view.bounds.size.width;
    CGFloat videoWindowHeight = videoWindowWidth * 9 / 16;
    videoWindow = [[VideoWindow alloc]initWithFrame:CGRectMake(0, 0, videoWindowWidth, videoWindowHeight)];
    [self.view addSubview:videoWindow];
}

- (IBAction)playPauseClicked:(UIButton *)sender {
    if(loadMedia == NO) {/* first playback */
        /* Hongkong TV */
        NSString* mediaURL = [[[NSBundle mainBundle] bundlePath] stringByAppendingPathComponent:@"test.mp4"];
        NSInteger ret = [playerAPI openMediaPath:mediaURL
                                     videoWindow:videoWindow
                                     windowWidth:videoWindow.frame.size.width
                                    windowHeight:videoWindow.frame.size.height];
        if(ret == EPlayer_Err_None) {
            loadMedia = YES;
            [playerAPI play];
            [_playPause setTitle:@"Pause" forState:UIControlStateNormal];
        }
    }
    else {
        EPlayerStatus status = [playerAPI getPlayerStatus];
        if(status == EPlayerStatus_Paused) {
            [playerAPI play];
            [_playPause setTitle:@"Pause" forState:UIControlStateNormal];
        }
        else {
            [playerAPI pause];
            [_playPause setTitle:@"Play" forState:UIControlStateNormal];
        }
    }
}

@end
