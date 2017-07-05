/*
 * ****************************************************************
 * This software is a media player SDK implementation
 * GPL:
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details. You should
 * have received a copy of the GNU Library General Public License
 * along with this library; if not, write to the Free Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Project: EC < Enjoyable Coding >
 *
 * VideoWindow.mm
 * This file is VideoWindow interface implementation
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#import "VideoWindow.h"
#import "VideoRenderLayer.h"

@interface VideoWindow ()
{
    CGFloat                 _videoWidth;
    CGFloat                 _videoHeight;
    CGFloat                 _userWndWidt;
    CGFloat                 _userWndHeight;
    CGFloat                 _viewScale;
    CGFloat                 _drawVideo_x;
    CGFloat                 _drawVideo_y;
    CGFloat                 _drawVideo_w;
    CGFloat                 _drawVideo_h;
    CVPixelBufferRef        _pixBufRef;
    VideoRotation           _rotation;
    VideoRenderLayer*       _drawFrameLayer;
}

- (id)initWithFrame:(CGRect)frame;
- (id)initWithCoder:(NSCoder*)aDecoder;
- (void)clearWindow;
- (void)setRotation:(VideoRotation)rotation;
- (void)drawPixelBuffer:(CVPixelBufferRef)pixelBuffer width:(CGFloat)width height:(CGFloat)height;
- (void)drawYUV:(void *)YBuf U:(void *)UBuf V:(void *)VBuf strideY:(CGFloat)strideY strideUV:(CGFloat)strideUV frameWidth:(CGFloat)width frameHeight:(CGFloat)height;
- (void)setRenderParam:(CGFloat)videoWidth videoHeight:(CGFloat)videoHeight
       userWindowWidth:(CGFloat)userWndWidth userWindowHeight:(CGFloat)userWndHeight;
@end

@implementation VideoWindow
- (id)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    _drawFrameLayer = [[VideoRenderLayer alloc] initWithFrame:self.bounds];
    [self.layer insertSublayer:_drawFrameLayer atIndex:0];
    return self;
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    _viewScale = [[UIScreen mainScreen] scale];
    _drawFrameLayer = [[VideoRenderLayer alloc] initWithFrame:self.bounds];
    [self.layer insertSublayer:_drawFrameLayer atIndex:0];
    return self;
}

- (void)initVideoRendLayer
{
}

- (void)drawPixelBuffer:(CVPixelBufferRef)pixelBuffer width:(CGFloat)width height:(CGFloat)height
{
    [_drawFrameLayer drawPixelBuffer:pixelBuffer width:width height:height];
}

- (CVPixelBufferRef)createPixBufferFromYUV:(uint8_t*)Y
                                       uBuf:(uint8_t*)U
                                       vBuf:(uint8_t*)V
                                    strideY:(CGFloat)strideY
                                   strideUV:(CGFloat)strideUV
                                 frameWidth:(CGFloat)width
                                frameHeight:(CGFloat)height
{
    if(_videoWidth != width || _videoHeight != height)
    {
        if(_pixBufRef != nil)
        {
            CVPixelBufferRelease(_pixBufRef);
            _pixBufRef = nil;
        }
    }
    if(_pixBufRef == nil)
    {
        NSDictionary *options = [NSDictionary dictionaryWithObjectsAndKeys:
                                 @(strideY), kCVPixelBufferBytesPerRowAlignmentKey,
                                 [NSNumber numberWithBool:YES], kCVPixelBufferOpenGLESCompatibilityKey,
                                 [NSDictionary dictionary], kCVPixelBufferIOSurfacePropertiesKey,
                                 nil];
        int ret = CVPixelBufferCreate(kCFAllocatorDefault,
                                      width, height,
                                      kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange,
                                      (__bridge CFDictionaryRef)(options),
                                      &_pixBufRef);
        if(ret != kCVReturnSuccess)
        {
            return nil;
        }
    }
    size_t srcPlaneSize = strideUV * height/2;
    size_t dstPlaneSize = srcPlaneSize * 2;
    uint8_t *dstPlane = (uint8_t*)malloc(dstPlaneSize);

    // interleave Cb and Cr plane [YUV420P]
    for(size_t i = 0; i < srcPlaneSize; i++)
    {
        dstPlane[2*i  ]= U[i];
        dstPlane[2*i+1]= V[i];
    }

    CVPixelBufferLockBaseAddress(_pixBufRef, 0);

    size_t bytePerRowY = CVPixelBufferGetBytesPerRowOfPlane(_pixBufRef, 0);
    size_t bytesPerRowUV = CVPixelBufferGetBytesPerRowOfPlane(_pixBufRef, 1);

    void* base =  CVPixelBufferGetBaseAddressOfPlane(_pixBufRef, 0);
    memcpy(base, Y, bytePerRowY * height);
    base = CVPixelBufferGetBaseAddressOfPlane(_pixBufRef, 1);
    memcpy(base, dstPlane, bytesPerRowUV * height/2);

    CVPixelBufferUnlockBaseAddress(_pixBufRef, 0);
    free(dstPlane);

    return _pixBufRef;
}

- (void)drawYUV:(void *)YBuf
              U:(void *)UBuf
              V:(void *)VBuf
        strideY:(CGFloat)strideY
       strideUV:(CGFloat)strideUV
     frameWidth:(CGFloat)width
    frameHeight:(CGFloat)height
{
    CVPixelBufferRef pixBufRef = [self createPixBufferFromYUV:(uint8_t*)YBuf
                                                          uBuf:(uint8_t*)UBuf
                                                          vBuf:(uint8_t*)VBuf
                                                       strideY:strideY
                                                      strideUV:strideUV
                                                    frameWidth:width
                                                   frameHeight:height];
    if(pixBufRef != nil)
    {
        [self drawPixelBuffer:pixBufRef width:width height:height];
    }
}

- (void)setRenderParam:(CGFloat)videoWidth videoHeight:(CGFloat)videoHeight
       userWindowWidth:(CGFloat)userWndWidth userWindowHeight:(CGFloat)userWndHeight
{
    _videoWidth = videoWidth;
    _videoHeight = videoHeight;
    _userWndWidt = userWndWidth;
    _userWndHeight = userWndHeight;
    if(userWndWidth != 0 && userWndHeight != 0)
    {
        [self ClipDrawVideoRect:_userWndWidt height:_userWndHeight];
    }
    [_drawFrameLayer setRenderFrame:CGRectMake(_drawVideo_x, _drawVideo_y, _drawVideo_w, _drawVideo_h)];
}

- (void)ClipDrawVideoRect:(CGFloat)wndWidth height:(CGFloat)wndHeight
{
    CGFloat videoScaleWidth = 0.0;
    CGFloat videoScaleHeight = 0.0;
    CGFloat viewWidth = wndWidth;
    CGFloat viewHeight = wndHeight;
    GLfloat viewAspectRatio = (GLfloat)viewWidth / (GLfloat)viewHeight;
    GLfloat videoAspectRatio = 0.0f;
    if((_rotation == VideoRotation_None)     ||
       (_rotation == VideoRotation_Left_180) ||
       (_rotation == VideoRotation_Right_180) )
    {
        videoAspectRatio = (GLfloat)_videoWidth / (GLfloat)_videoHeight;
    }
    else
    {
        videoAspectRatio = (GLfloat)_videoHeight / (GLfloat)_videoWidth;
    }

    if (viewAspectRatio > videoAspectRatio)
    {
        videoScaleHeight = viewHeight;
        videoScaleWidth = viewHeight * videoAspectRatio;
        _drawVideo_y = 0;
        _drawVideo_x = (viewWidth - videoScaleWidth) * 0.5;
    }
    else if (viewAspectRatio < videoAspectRatio)
    {
        videoScaleWidth = viewWidth;
        videoScaleHeight = viewWidth / videoAspectRatio;
        _drawVideo_x = 0;
        _drawVideo_y = (viewHeight - videoScaleHeight) * 0.5;
    }
    else
    {
        _drawVideo_x = 0;
        _drawVideo_y = 0;
        videoScaleWidth = viewWidth;
        videoScaleHeight = viewHeight;
    }
    _drawVideo_x = _drawVideo_x * _viewScale;
    _drawVideo_y = _drawVideo_y * _viewScale;
    _drawVideo_w = videoScaleWidth * _viewScale;
    _drawVideo_h = videoScaleHeight * _viewScale;
}

- (void)setRotation:(VideoRotation)rotation
{
    CGFloat rota = 0;
    if(rotation == VideoRotation_Right_90)
    {
        rota = -90;
        _rotation = VideoRotation_Right_90;
    }
    else {/* TODO */}
    [_drawFrameLayer setDrawRotation:rota];
    [_drawFrameLayer initOpenGLES];
}

- (void)clearWindow
{
    if(_pixBufRef != nil)
    {
        CVPixelBufferRelease(_pixBufRef);
        _pixBufRef = nil;
    }
}

@end
