/*
* This software is developed for study and improve coding skill ...
*
* Project:  Enjoyable Coding< EC >
* Copyright (C) 2014-2016 Gao Peng

* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Library General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.

* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.

* You should have received a copy of the GNU Library General Public
* License along with this library; if not, write to the Free
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

* ---------------------------------------------------------------------
* iOS_VideoRender.h
* This file is iOS_VideoRender class implementation
*
* Eamil:   epengao@126.com
* Author:  Peter Gao
* Version: Intial first version.
* --------------------------------------------------------------------
*/

#include "Source.h"
#include "VideoRend.h"
#include "iOS_VideoRender.h"
#include <UIKit/UIKit.h>
#include <Foundation/Foundation.h>

iOS_VideoRender::iOS_VideoRender()
:m_nVideoWidth(0)
,m_nVideoHeight(0)
,m_pVideoScreen(EC_NULL)
,m_pImgConvertCtx(EC_NULL)
{
}

iOS_VideoRender::~iOS_VideoRender()
{
}

EC_U32 iOS_VideoRender::Init(MediaCtxInfo* pMediaInfo,
                             EC_U32 nScreenWidth, 
                             EC_U32 nScreenHight, 
                             EC_VOIDP pDrawable)
{
    if((EC_NULL == pMediaInfo) ||
       (EC_NULL == pDrawable) ) return EC_Err_BadParam;

    m_pVideoScreen = pDrawable;
    m_nVideoWidth = pMediaInfo->m_nVideoWidth;
    m_nVideoHeight = pMediaInfo->m_nVideoHeight;
    avpicture_alloc(&m_sAVPicture, AV_PIX_FMT_RGB24, m_nVideoWidth, m_nVideoHeight);

    m_pImgConvertCtx = sws_getContext(m_nVideoWidth, m_nVideoHeight,
                                      PIX_FMT_YUV420P,
                                      m_nVideoWidth, m_nVideoHeight,
                                      AV_PIX_FMT_RGB24,
                                      SWS_FAST_BILINEAR,
                                      NULL, NULL, NULL);
    return Video_Render_Err_None;
}

EC_VOID iOS_VideoRender::Uninit()
{
    m_nVideoWidth = 0;
    m_nVideoHeight = 0;
    m_pVideoScreen = EC_NULL;
    avpicture_free(&m_sAVPicture);
    sws_freeContext(m_pImgConvertCtx);
}

EC_VOID iOS_VideoRender::Stop()
{
    m_pVideoScreen = EC_NULL;
}

EC_VOID iOS_VideoRender::ClearScreen()
{
}

EC_VOID iOS_VideoRender::DrawFrame(VideoFrame* pFrame)
{
    if(EC_NULL == m_pVideoScreen) return;

//    dispatch_sync(dispatch_get_main_queue(),^
//    {
//        AVFrame *pAVFrame = (AVFrame*)pFrame;
//        sws_scale(m_pImgConvertCtx,
//                  pAVFrame->data, pAVFrame->linesize, 0,
//                  pAVFrame->height, m_sAVPicture.data, m_sAVPicture.linesize);
//        CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault;
//        CFDataRef data = CFDataCreateWithBytesNoCopy(kCFAllocatorDefault,
//                                                     m_sAVPicture.data[0],
//                                                     m_sAVPicture.linesize[0] * m_nVideoHeight,
//                                                     kCFAllocatorNull);
//        CGDataProviderRef provider = CGDataProviderCreateWithCFData(data);
//        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
//        CGImageRef cgImage = CGImageCreate(m_nVideoWidth,
//                                           m_nVideoHeight,
//                                           8,
//                                           24,
//                                           m_sAVPicture.linesize[0],
//                                           colorSpace,
//                                           bitmapInfo,
//                                           provider,
//                                           NULL,
//                                           NO,
//                                           kCGRenderingIntentDefault);
//        UIImage *image = [UIImage imageWithCGImage:cgImage];
//        CGImageRelease(cgImage);
//        CGColorSpaceRelease(colorSpace);
//        CGDataProviderRelease(provider);
//        CFRelease(data);
//
//        UIImageView *screen = (__bridge UIImageView*)m_pVideoScreen;
//        if((image != nil) && (screen != nil))
//        {
//            [screen setImage:image];
//        }
//    });

    
    
    AVFrame *pAVFrame = (AVFrame*)pFrame;
    sws_scale(m_pImgConvertCtx,
              pAVFrame->data, pAVFrame->linesize, 0,
              pAVFrame->height, m_sAVPicture.data, m_sAVPicture.linesize);
    CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault;
    CFDataRef data = CFDataCreateWithBytesNoCopy(kCFAllocatorDefault,
                                                 m_sAVPicture.data[0],
                                                 m_sAVPicture.linesize[0] * m_nVideoHeight,
                                                 kCFAllocatorNull);
    CGDataProviderRef provider = CGDataProviderCreateWithCFData(data);
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGImageRef cgImage = CGImageCreate(m_nVideoWidth,
                                       m_nVideoHeight,
                                       8,
                                       24,
                                       m_sAVPicture.linesize[0],
                                       colorSpace,
                                       bitmapInfo,
                                       provider,
                                       NULL,
                                       NO,
                                       kCGRenderingIntentDefault);


    dispatch_sync(dispatch_get_main_queue(),^
    {
        UIImage *image = [UIImage imageWithCGImage:cgImage];
        UIImageView *screen = (__bridge UIImageView*)m_pVideoScreen;
        if((image != nil) && (screen != nil))
            [screen setImage:image];
    });

    CGImageRelease(cgImage);
    CGColorSpaceRelease(colorSpace);
    CGDataProviderRelease(provider);
    CFRelease(data);
}

EC_VOID iOS_VideoRender::UpdateVideoScreen(MediaEngVideoScreen *pVideoScreen)
{
    m_pVideoScreen = pVideoScreen->pScreen;
}
