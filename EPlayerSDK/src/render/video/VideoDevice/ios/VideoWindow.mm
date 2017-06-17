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
}

- (void)render;
- (void)loadShader;
- (void)clearWindow;
- (void)setupYUVTexture;
- (BOOL)createFrameAndRenderBuffer;
- (void)destoryFrameAndRenderBuffer;
- (void)ClipDrawVideoRect:(CGFloat)wndWidth height:(CGFloat)wndHeight;
- (void)drawYUV:(void *)YBuf U:(void *)UBuf V:(void *)VBuf;
- (GLuint)compileShader:(NSString*)shaderCode withType:(GLenum)shaderType;
@end

@implementation VideoWindow
- (BOOL)EGL_Init
{
    CAEAGLLayer *eaglLayer = (CAEAGLLayer*) self.layer;
    eaglLayer.opaque = YES;
    eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                    [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking,
                                    kEAGLColorFormatRGB565, kEAGLDrawablePropertyColorFormat, nil];
    self.contentScaleFactor = [UIScreen mainScreen].scale;
    _viewScale = [UIScreen mainScreen].scale;
    _glContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    if(!_glContext || ![EAGLContext setCurrentContext:_glContext])
    {
        return NO;
    }
    [self setupYUVTexture];
    [self loadShader];
    glUseProgram(_program);
    GLuint textureUniformY = glGetUniformLocation(_program, "SamplerY");
    GLuint textureUniformU = glGetUniformLocation(_program, "SamplerU");
    GLuint textureUniformV = glGetUniformLocation(_program, "SamplerV");
    glUniform1i(textureUniformY, Y);
    glUniform1i(textureUniformU, U);
    glUniform1i(textureUniformV, V);
    _rotation = VideoRotation_None;

    return YES;
}

- (id)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if (self)
    {
        if (![self EGL_Init])
        {
            self = nil;
        }
    }
    return self;
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self)
    {
        if (![self EGL_Init])
        {
            self = nil;
        }
    }
    return self;
}

- (void)drawYUV:(void *)YBuf U:(void *)UBuf V:(void *)VBuf
{
    @synchronized(self)
    {
        [EAGLContext setCurrentContext:_glContext];
        glBindTexture(GL_TEXTURE_2D, _textureYUV[Y]);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _videoWidth, _videoHeight, GL_RED_EXT, GL_UNSIGNED_BYTE, YBuf);
        glBindTexture(GL_TEXTURE_2D, _textureYUV[U]);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _videoWidth/2, _videoHeight/2, GL_RED_EXT, GL_UNSIGNED_BYTE, UBuf);
        glBindTexture(GL_TEXTURE_2D, _textureYUV[V]);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _videoWidth/2, _videoHeight/2, GL_RED_EXT, GL_UNSIGNED_BYTE, VBuf);
        [self render];
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
    if ([self window])
    {
        [EAGLContext setCurrentContext:_glContext];
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindRenderbuffer(GL_RENDERBUFFER, _renderBuffer);
        [_glContext presentRenderbuffer:GL_RENDERBUFFER];
    }
}

- (void)layoutSubviews
{
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        @synchronized(self)
        {
            [EAGLContext setCurrentContext:_glContext];
            [self destoryFrameAndRenderBuffer];
            [self createFrameAndRenderBuffer];
        }
    });
}

- (void)render
{
    [EAGLContext setCurrentContext:_glContext];
    glViewport(_drawVideo_x, _drawVideo_y, _drawVideo_w, _drawVideo_h);
    static const GLfloat squareVertices[] = {-1,-1, 1,-1, -1,1, 1,1,};
    static const GLfloat coordVertices_None[] = {0,1, 1,1, 0,0, 1,0,};
    static const GLfloat coordVertices_R90[] = {1,1, 1,0, 0,1, 0,0,};
    static const GLfloat *coordVertices = NULL;
    if(_rotation == VideoRotation_None)
    {
        coordVertices = coordVertices_None;
    }
    else if(_rotation == VideoRotation_Right_90)
    {
        coordVertices = coordVertices_R90;
    }
    else {/* TODO */}

    glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, squareVertices);
    glEnableVertexAttribArray(ATTRIB_VERTEX);
    glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, 0, 0, coordVertices);
    glEnableVertexAttribArray(ATTRIB_TEXTURE);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindRenderbuffer(GL_RENDERBUFFER, _renderBuffer);
    [_glContext presentRenderbuffer:GL_RENDERBUFFER];
}

#pragma mark ELG Settings
+ (Class)layerClass
{
    return [CAEAGLLayer class];
}

- (void)setupYUVTexture
{
    if (_textureYUV[Y])
    {
        glDeleteTextures(YUV, _textureYUV);
    }
    glGenTextures(YUV, _textureYUV);
    if (!_textureYUV[Y] || !_textureYUV[U] || !_textureYUV[V])
    {
        return;
    }
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureYUV[Y]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glActiveTexture(GL_TEXTURE1);
    
    glBindTexture(GL_TEXTURE_2D, _textureYUV[U]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glActiveTexture(GL_TEXTURE2);
    
    glBindTexture(GL_TEXTURE_2D, _textureYUV[V]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

- (BOOL)createFrameAndRenderBuffer
{
    glGenFramebuffers(1, &_framebuffer);
    glGenRenderbuffers(1, &_renderBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _renderBuffer);
    if (![_glContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)self.layer])
    {
        return NO;
    }
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _renderBuffer);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        return NO;
    }
    return YES;
}

- (void)destoryFrameAndRenderBuffer
{
    if (_framebuffer)
    {
        glDeleteFramebuffers(1, &_framebuffer);
    }
    if (_renderBuffer)
    {
        glDeleteRenderbuffers(1, &_renderBuffer);
    }
    _framebuffer = 0;
    _renderBuffer = 0;
}

/*
1,1,1, 0, -0.34413,1.772,  1.402,   -0.71414,0 SDTV
1,1,1, 0, -0.39465,2.03211,1.13983, -0.58060,0
1,1,1, 0, -0.18732,1.8556, 1.57481, -0.46813,0 HDTV
*/
#define FSH @"varying lowp vec2 TexCoordOut;\
\
uniform sampler2D SamplerY;\
uniform sampler2D SamplerU;\
uniform sampler2D SamplerV;\
\
void main(void)\
{\
mediump vec3 yuv;\
lowp vec3 rgb;\
\
yuv.x = texture2D(SamplerY, TexCoordOut).r;\
yuv.y = texture2D(SamplerU, TexCoordOut).r - 0.5;\
yuv.z = texture2D(SamplerV, TexCoordOut).r - 0.5;\
\
rgb = mat3(1,1,1, 0, -0.18732,1.8556, 1.57481, -0.46813,0) * yuv;\
\
gl_FragColor = vec4(rgb, 1);\
}"

#define VSH @"attribute vec4 position;\
attribute vec2 TexCoordIn;\
varying vec2 TexCoordOut;\
\
void main(void)\
{\
gl_Position = position;\
TexCoordOut = TexCoordIn;\
}"

- (void)loadShader
{
    GLuint vertexShader = [self compileShader:VSH withType:GL_VERTEX_SHADER];
    GLuint fragmentShader = [self compileShader:FSH withType:GL_FRAGMENT_SHADER];
    _program = glCreateProgram();
    glAttachShader(_program, vertexShader);
    glAttachShader(_program, fragmentShader);
    glBindAttribLocation(_program, ATTRIB_VERTEX, "position");
    glBindAttribLocation(_program, ATTRIB_TEXTURE, "TexCoordIn");
    glLinkProgram(_program);
    GLint linkSuccess;
    glGetProgramiv(_program, GL_LINK_STATUS, &linkSuccess);
    if (linkSuccess == GL_FALSE)
    {
        GLchar messages[256];
        glGetProgramInfoLog(_program, sizeof(messages), 0, &messages[0]);
    }
    if (vertexShader)
    {
        glDeleteShader(vertexShader);
    }
    if (fragmentShader)
    {
        glDeleteShader(fragmentShader);
    }
}

- (GLuint)compileShader:(NSString*)shaderString withType:(GLenum)shaderType
{
    if (!shaderString)
    {
        return -1;
    }
    GLuint shaderHandle = glCreateShader(shaderType);
    const char * shaderStringUTF8 = [shaderString UTF8String];
    int shaderStringLength = (int)[shaderString length];
    glShaderSource(shaderHandle, 1, &shaderStringUTF8, &shaderStringLength);
    glCompileShader(shaderHandle);

    GLint compileSuccess;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compileSuccess);
    if (compileSuccess == GL_FALSE)
    {
        GLchar messages[256];
        glGetShaderInfoLog(shaderHandle, sizeof(messages), 0, &messages[0]);
        return -1;
    }
    return shaderHandle;
}
@end
