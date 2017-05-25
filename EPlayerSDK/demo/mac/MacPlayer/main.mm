//
//  main.cpp
//  EPlayer
//
//  Created by Anthon Liu on 2017/4/13.
//  Copyright © 2017年 EC. All rights reserved.
//

#include <unistd.h>
#include <iostream>
#include "EPlayerAPI.h"

#import <Cocoa/Cocoa.h>

#define W_WIDTH  202
#define W_HEIGHT 301

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

using namespace std;

static void _switch_media_time(int time_ms)
{
    int toal_s = time_ms / 1000;
    int hh = toal_s / 3600;
    int mm = (toal_s % 3600) / 60;
    int ss = toal_s % 60;

    printf("[%d:%d:%d]", hh, mm, ss);
}

int main(int argc, const char * argv[]) {

    EPlayerAPI playerAPI;
    InitEPlayerAPI(&playerAPI);

    NSWindow *screen = creat_window(W_WIDTH , W_HEIGHT);
    //NSWindow *screen_bak = creat_window(200, 100);
    void* drawable = (__bridge void *)(screen);
    //int xxx = player.OpenMedia("http://wsod.qingting.fm/vod/00/00/0000000000000000000024714128_24.m4a");
    //int xxx = player.OpenMedia("http://live.whtv.com.cn/live/2a532d70bcbe47e6aaccc74dea2655cc?fmt=h264_450k_flv", drawable, 600, 400);

    playerAPI.Init();
    int xxx = playerAPI.OpenMedia("/Users/epengao/Downloads/111.dat", drawable, W_WIDTH, W_HEIGHT);
    if(xxx)
    {
        printf("Open Faild: 0x%08X\n", xxx);
    }
    else
    {
        char c;
        int i = 0;
        cout<<"Open Media OK"<<endl;
        playerAPI.Play();
        while (1) {
            int playTime = playerAPI.GetPlayingPos();
            int buffTime = playerAPI.GetBufferingPos();
            _switch_media_time(playTime);
            //cout<<" ";
            //_switch_media_time(buffTime);
            cout<<endl;
            sleep(1);
            i++;
            if(i == 8)
            {
                playerAPI.Seek(136 * 1000);
            }
            else if(i == 15)
            {
                
            }
            else if(i == 20)
            {
                //playerAPI.Play();
            }
            /* resize */
            if(10 < i && i < 20)
            {
                //playerAPI.ResizeVidoeScreen(300, 300);
            }
            else if(i == 20)
            {
                //playerAPI.ResizeVidoeScreen(200+200%8, 200+200%8);
            }
        }

        cin>>c;
        playerAPI.CloseMedia();
    }

    return 0;
}
