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

enum EGL_Attrib
{
    ATTRIB_VERTEX,
    ATTRIB_TEXTURE,
    ATTRIB_COLOR,
};

enum EGL_Texture
{
    Y   = 0,
    U   = 1,
    V   = 2,
    YUV = 3,
};

@interface VideoWindow ()
{
    EAGLContext             *_glContext;
    GLuint                  _framebuffer;
    GLuint                  _renderBuffer;
    GLuint                  _program;
    GLuint                  _textureYUV[YUV];
    CGFloat                 _videoWidth;
    CGFloat                 _videoHeight;
    CGFloat                 _userWndWidt;
    CGFloat                 _userWndHeight;
    CGFloat                 _viewScale;
    CGFloat                 _drawVideo_x;
    CGFloat                 _drawVideo_y;
    CGFloat                 _drawVideo_w;
    CGFloat                 _drawVideo_h;
    VideoRotation           _rotation;
    VideoRenderLayer*       _drawFrameLayer;
}

- (id)initWithFrame:(CGRect)frame;
- (id)initWithCoder:(NSCoder*)aDecoder;
- (void)clearWindow;
- (void)setRotation:(VideoRotation)rotation;
- (void)drawPixelBuffer:(CVPixelBufferRef)pixelBuffer;
- (void)drawYUV:(void *)YBuf U:(void *)UBuf V:(void *)VBuf;
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
    _drawFrameLayer = [[VideoRenderLayer alloc] initWithFrame:self.bounds];
    [self.layer insertSublayer:_drawFrameLayer atIndex:0];
    return self;
}

- (void)drawPixelBuffer:(CVPixelBufferRef)pixelBuffer
{
    dispatch_sync(dispatch_get_main_queue(), ^{
        [_drawFrameLayer drawPixelBuffer:pixelBuffer];
    });
}

- (void)drawYUV:(void *)YBuf U:(void *)UBuf V:(void *)VBuf
{
    @synchronized(self)
    {
    }
}

- (void)setRenderParam:(CGFloat)videoWidth videoHeight:(CGFloat)videoHeight
       userWindowWidth:(CGFloat)userWndWidth userWindowHeight:(CGFloat)userWndHeight
{
    _viewScale = 1.0;
    _videoWidth = videoWidth;
    _videoHeight = videoHeight;
    _userWndWidt = userWndWidth;
    _userWndHeight = userWndHeight;
    if(userWndWidth != 0 && userWndHeight != 0)
    {
        [self ClipDrawVideoRect:_userWndWidt height:_userWndHeight];
    }
    GLuint renderDataSize = _videoWidth * _videoHeight;
    GLubyte *blackData = (GLubyte*)malloc(renderDataSize * 1.5);
    if(blackData)
    {
        memset(blackData, 0x0, renderDataSize * 1.5);
    }
    [EAGLContext setCurrentContext:_glContext];
    glBindTexture(GL_TEXTURE_2D, _textureYUV[Y]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED_EXT, _videoWidth, _videoHeight, 0, GL_RED_EXT, GL_UNSIGNED_BYTE, blackData);
    glBindTexture(GL_TEXTURE_2D, _textureYUV[U]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED_EXT, _videoWidth/2, _videoHeight/2, 0, GL_RED_EXT, GL_UNSIGNED_BYTE, blackData + renderDataSize);
    glBindTexture(GL_TEXTURE_2D, _textureYUV[V]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED_EXT, _videoWidth/2, _videoHeight/2, 0, GL_RED_EXT, GL_UNSIGNED_BYTE, blackData + renderDataSize * 5 / 4);
    free(blackData);

    [_drawFrameLayer setDrawRect:CGRectMake(_drawVideo_x, _drawVideo_y, _drawVideo_w, _drawVideo_h)];
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
    if(rotation == VideoRotation_Right_90)
    {
        _rotation = VideoRotation_Right_90;
        [self setRenderParam:_videoWidth videoHeight:_videoHeight
             userWindowWidth:_userWndWidt userWindowHeight:_userWndHeight];
    }
    else {/* TODO */}
}

- (void)clearWindow
{
}

@end
