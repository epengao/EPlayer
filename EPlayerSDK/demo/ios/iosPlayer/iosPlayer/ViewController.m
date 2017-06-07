//
//  ViewController.m
//  iosPlayer
//
//  Created by GaoPeng on 2017/6/5.
//  Copyright © 2017年 EC. All rights reserved.
//

#import "EPlayerAPI.h"
#import "ViewController.h"

@interface ViewController ()
{
    VideoWindow* videoWnd;
    EPlayerAPI*  playerAPI;
    MediaInfo*   medaiInfo;

    NSString *videoFilesFolder;
    NSArray *videoFileNameList;
}
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    videoWnd = [[VideoWindow alloc]initWithFrame:CGRectMake(0, 0, self.view.frame.size.width, self.view.frame.size.width*9/16)];
    [self.view addSubview:videoWnd];
    [self getAllVideoFiles];
    playerAPI = [EPlayerAPI sharedEPlayerAPI];
    if(playerAPI != nil)
    {
        if(videoFilesFolder != nil && [videoFileNameList count] > 0)
        {
            NSString* mediaURL = [NSString stringWithFormat:@"%@/%@", videoFilesFolder, videoFileNameList[0]];
            NSInteger ret = [playerAPI openMediaPath:mediaURL
                                   videoWindow:videoWnd
                                   windowWidth:videoWnd.frame.size.width
                                  windowHeight:videoWnd.frame.size.height];
            if(ret == EPlayer_Err_None)
            {
                [playerAPI play];
            }
        }
    }
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)getAllVideoFiles
{
    NSArray *patchs = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [patchs objectAtIndex:0];
    NSString *fileDirectory = [documentsDirectory stringByAppendingPathComponent:@""];
    videoFilesFolder = fileDirectory;
    videoFileNameList = [[NSFileManager defaultManager] subpathsOfDirectoryAtPath:fileDirectory error:nil];
}

@end
