//
//  ViewController.m
//  iosPlayer
//
//  Created by GaoPeng on 2017/6/5.
//  Copyright © 2017年 EC. All rights reserved.
//

#import "EPlayerAPI.h"
#import "ViewController.h"
#import "PlayVideoViewController.h"

@interface ViewController ()
{
    VideoWindow* videoWnd;
    NSString *videoFilesFolder;
    NSArray *videoFileNameList;
}
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)playVideoClicked:(id)sender
{
    static NSUInteger playOneTow = 0;
    [self getAllVideoFiles];
    if(videoFilesFolder != nil && [videoFileNameList count] > 0)
    {
        NSString* mediaURL = [NSString stringWithFormat:@"%@/%@", videoFilesFolder, videoFileNameList[0]];
        PlayVideoViewController *playView = [[PlayVideoViewController alloc]init];
        playView.videoFileURL = mediaURL;
        [self presentViewController:playView animated:YES completion:nil];
    }
    playOneTow = playOneTow + 1;
}

- (void)getAllVideoFiles
{
    NSArray *patchs = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [patchs objectAtIndex:0];
    NSString *fileDirectory = [documentsDirectory stringByAppendingPathComponent:@""];
    videoFilesFolder = fileDirectory;
    videoFileNameList = [[NSFileManager defaultManager] subpathsOfDirectoryAtPath:fileDirectory error:nil];
}

- (BOOL)shouldAutorotate
{
    return YES;
}

- (UIInterfaceOrientationMask)supportedInterfaceOrientations
{
    return UIInterfaceOrientationMaskPortrait | UIInterfaceOrientationMaskLandscapeLeft| UIInterfaceOrientationMaskLandscapeRight;
}

@end
