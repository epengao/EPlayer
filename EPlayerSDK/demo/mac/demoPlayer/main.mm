//
//  main.m
//  demoPlayer
//
//  Created by GaoPeng on 2017/5/27.
//  Copyright © 2017年 EC. All rights reserved.
//

#import "EPlayerAPI.h"
#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>

#define W_WIDTH  907
#define W_HEIGHT 617

NSWindow* creat_window(int w, int h)
{
    [NSApplication sharedApplication];
    NSRect windowRect = NSMakeRect(100, 100, w, h);
    NSUInteger windowStyle = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable;
    NSWindow * window = [[NSWindow alloc] initWithContentRect:windowRect
                                                    styleMask:windowStyle
                                                      backing:NSBackingStoreBuffered
                                                        defer:NO];
    [window orderFrontRegardless];
    return window;
}

static void print_media_time(int time_ms)
{
    int toal_s = time_ms / 1000;
    int hh = toal_s / 3600;
    int mm = (toal_s % 3600) / 60;
    int ss = toal_s % 60;

    printf("[%d:%d:%d]\n", hh, mm, ss);
}

EPlayerAPI *playerAPI;

int main(int argc, const char * argv[])
{
    playerAPI = [EPlayerAPI sharedEPlayerAPI];
    NSString* mediaPath = @"/User/EPlayer/mediaTest.mkv";
    NSWindow *videoScreen = creat_window(W_WIDTH , W_HEIGHT);

    NSInteger nRet = [playerAPI openMediaPath:mediaPath
                                  videoWindow:(__bridge void*)videoScreen
                                  windowWidth:W_WIDTH
                                 windowHeight:W_HEIGHT];
    if(nRet == EPlayer_Err_None)
    {
        [playerAPI play];
        while (true)
        {
            NSInteger playtime = [playerAPI getPlayingPos];
            print_media_time((int)playtime);
            sleep(1);
        }
    }

    return 0;
}
