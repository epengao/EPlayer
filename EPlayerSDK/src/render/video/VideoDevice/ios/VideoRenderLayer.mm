
#import <mach/mach_time.h>
#import <UIKit/UIScreen.h>
#include <OpenGLES/EAGL.h>
#import "VideoRenderLayer.h"
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#import <AVFoundation/AVUtilities.h>
#include <AVFoundation/AVFoundation.h>

// Uniform index.
enum
{
    UNIFORM_Y,
    UNIFORM_UV,
    //UNIFORM_ROTATION_ANGLE,
    //UNIFORM_COLOR_CONVERSION_MATRIX,
    UNIFORM_TEXMATRIX,
    UNIFORM_MVPMATRIX,
    UNIFORM_COLOR_CONVERSION_MATRIX,
    NUM_UNIFORMS
};
GLint uniforms[NUM_UNIFORMS];

typedef struct
{
    float Position[2];
    float TexCoord[2];
} GLVertex;

// Attribute index.
enum
{
    ATTRIB_VERTEX,
    ATTRIB_TEXCOORD,
    NUM_ATTRIBUTES
};

static const GLVertex Vertices[] =
{
    {{-1, -1}, {0, 1}},
    {{-1,  1}, {0, 0}},
    {{ 1, -1}, {1, 1}},
    {{ 1,  1}, {1, 0}}
};

static const GLVertex Vertices_R90[] =
{
    {{-1, -1}, {1, 1}},
    {{-1,  1}, {0, 1}},
    {{ 1, -1}, {1, 0}},
    {{ 1,  1}, {0, 0}}
};

// Color Conversion Constants (YUV to RGB) including adjustment from 16-235/16-240 (video range)

// BT.601, which is the standard for SDTV.
static const GLfloat kColorConversion601[] =
{
    1.164,  1.164, 1.164,
    0.0,   -0.392, 2.017,
    1.596, -0.813,   0.0,
};

// BT.709, which is the standard for HDTV.
static const GLfloat kColorConversion709[] = {
    1.164,  1.164, 1.164,
    0.0,   -0.213, 2.112,
    1.793, -0.533,   0.0,
};

#define RenderBufferSize 1920*1080*3
@interface VideoRenderLayer ()
{
    // The pixel dimensions of the CAEAGLLayer.
    GLuint mVertexBuffer;
    uint8_t trueBuffer[RenderBufferSize];

    GLint _drawX;
    GLint _drawY;
    GLint _drawW;
    GLint _drawH;
    GLint _backingWidth;
    GLint _backingHeight;

    GLfloat _renderRotation;

    EAGLContext *_context;
    CVOpenGLESTextureRef _lumaTexture;
    CVOpenGLESTextureRef _chromaTexture;

    GLuint _frameBufferHandle;
    GLuint _colorBufferHandle;

    const GLfloat *_preferredConversion;
}
@property GLuint program;
@end

@implementation VideoRenderLayer

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super init];
    if (self)
    {
        CGFloat scale = [[UIScreen mainScreen] scale];
        self.contentsScale = scale;
        self.opaque = TRUE;
        self.drawableProperties = @{ kEAGLDrawablePropertyRetainedBacking :[NSNumber numberWithBool:YES]};

        [self setFrame:frame];

        // Set the context into which the frames will be drawn.
        _context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        if (!_context)
        {
            return nil;
        }
        // Set the default conversion to BT.709, which is the standard for HDTV.
        _preferredConversion = kColorConversion709;
    }
    return self;
}

- (void)setRenderFrame:(CGRect)frame
{
    _drawX = frame.origin.x;
    _drawY = frame.origin.y;
    _drawW = frame.size.width;
    _drawH = frame.size.height;
}

- (void)initOpenGLES
{
    [self setupGL];
}

- (void)prepareTextureForRGBRendering:(GLuint) texW textureHeight: (GLuint) texH frameWidth: (GLuint) frameW frameHeight: (GLuint) frameH
{
}

- (void)setDrawRotation:(CGFloat)rotation
{
    _renderRotation = rotation;
}

- (void)drawRGBBuffer:(uint8_t*)rgbBuffer
{
    [EAGLContext setCurrentContext:_context];

    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferHandle);
    glViewport(_drawX, _drawY, _drawW, _drawH);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // OpenGL loads textures lazily so accessing the buffer is deferred until draw; notify
    // the movie player that we're done with the texture after glDrawArrays.

    memcpy(trueBuffer, rgbBuffer, RenderBufferSize);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1920, 1080, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, trueBuffer);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindRenderbuffer(GL_RENDERBUFFER, _colorBufferHandle);
    [_context presentRenderbuffer:GL_RENDERBUFFER];
}

- (void)drawPixelBuffer:(CVPixelBufferRef)pixelBuffer width:(CGFloat)width height:(CGFloat)height
{
    if (!_context || ![EAGLContext setCurrentContext:_context])
    {
        return;
    }
    /* dump pix buffer format
    OSType type = CVPixelBufferGetPixelFormatType(pixelBuffer);
    char format[5];
    uint32_t typeSource = (uint32_t)type;
    format[0] = (typeSource >>24) & 0xff;
    format[1] = (typeSource >>16) & 0xff;
    format[2] = (typeSource >>8) & 0xff;
    format[3] = (typeSource) & 0xff;
    format[4] = '\0';
    NSLog(@"pixel buffer type: %s ", format);
    if (type == '420v')
    {
        NSLog(@"format is yuv");
    }
    */
    CVReturn err;
    size_t planeCount = CVPixelBufferGetPlaneCount(pixelBuffer);
    /*
     Use the color attachment of the pixel buffer to determine the appropriate color conversion matrix.
     */
    CFTypeRef colorAttachments = CVBufferGetAttachment(pixelBuffer, kCVImageBufferYCbCrMatrixKey, NULL);
    if (colorAttachments == kCVImageBufferYCbCrMatrix_ITU_R_601_4)
    {
        _preferredConversion = kColorConversion601;
    }
    else
    {
        _preferredConversion = kColorConversion709;
    }
    /*
     CVOpenGLESTextureCacheCreateTextureFromImage will create GLES texture optimally from CVPixelBufferRef.
     */
    /*
     Create Y and UV textures from the pixel buffer. These textures will be drawn on the frame buffer Y-plane.
     */
    CVOpenGLESTextureCacheRef _videoTextureCache;
    // Create CVOpenGLESTextureCacheRef for optimal CVPixelBufferRef to GLES texture conversion.
    err = CVOpenGLESTextureCacheCreate(kCFAllocatorDefault, NULL, _context, NULL, &_videoTextureCache);
    if (err != noErr)
    {
        NSLog(@"Error at CVOpenGLESTextureCacheCreate %d", err);
        return;
    }
    glActiveTexture(GL_TEXTURE0);
    err = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault,
                                                       _videoTextureCache,
                                                       pixelBuffer,
                                                       NULL,
                                                       GL_TEXTURE_2D,
                                                       GL_RED_EXT,
                                                       width,
                                                       height,
                                                       GL_RED_EXT,
                                                       GL_UNSIGNED_BYTE,
                                                       0,
                                                       &_lumaTexture);
    if (err)
    {
        NSLog(@"Error at CVOpenGLESTextureCacheCreateTextureFromImage for luma %d", err);
    }
    glBindTexture(CVOpenGLESTextureGetTarget(_lumaTexture), CVOpenGLESTextureGetName(_lumaTexture));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    if(planeCount == 2)
    {
        // UV-plane.
        glActiveTexture(GL_TEXTURE1);
        err = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault,
                                                           _videoTextureCache,
                                                           pixelBuffer,
                                                           NULL,
                                                           GL_TEXTURE_2D,
                                                           GL_RG_EXT,
                                                           width / 2,
                                                           height / 2,
                                                           GL_RG_EXT,
                                                           GL_UNSIGNED_BYTE,
                                                           1,
                                                           &_chromaTexture);
        if (err)
        {
            NSLog(@"Error at CVOpenGLESTextureCacheCreateTextureFromImage for chroma %d", err);
        }
        glBindTexture(CVOpenGLESTextureGetTarget(_chromaTexture), CVOpenGLESTextureGetName(_chromaTexture));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferHandle);
    // Set the view port to the entire view.
    glViewport(_drawX, _drawY, _drawW, _drawH);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    // Use shader program.
    glUseProgram(self.program);
    int mWidth = 1980;
    int mHeight = 1080;
    float aspectRatio = ((float)1280/(float)720) / ((float)mWidth/(float)mHeight);
    GLKMatrix4 modelViewProjectionMatrix = GLKMatrix4Scale(GLKMatrix4Identity, aspectRatio, -1, 1);
    GLKMatrix4 textureMatrix = GLKMatrix4Identity;

    glUniformMatrix4fv(uniforms[UNIFORM_MVPMATRIX], 1, false, modelViewProjectionMatrix.m);
    glUniformMatrix4fv(uniforms[UNIFORM_TEXMATRIX], 1, false, textureMatrix.m);
    //glUniform1f(uniforms[UNIFORM_ROTATION_ANGLE], 0);
    glUniformMatrix3fv(uniforms[UNIFORM_COLOR_CONVERSION_MATRIX], 1, GL_FALSE, _preferredConversion);
//    // Set up the quad vertices with respect to the orientation and aspect ratio of the video.
//    CGRect viewBounds = self.bounds;
//    CGSize contentSize = CGSizeMake(frameWidth, frameHeight);
//    CGRect vertexSamplingRect = AVMakeRectWithAspectRatioInsideRect(contentSize, viewBounds);
//    
//    // Compute normalized quad coordinates to draw the frame into.
//    CGSize normalizedSamplingSize = CGSizeMake(0.0, 0.0);
//    CGSize cropScaleAmount = CGSizeMake(vertexSamplingRect.size.width/viewBounds.size.width,
//                                        vertexSamplingRect.size.height/viewBounds.size.height);
//    
//    // Normalize the quad vertices.
//    if (cropScaleAmount.width > cropScaleAmount.height) {
//        normalizedSamplingSize.width = 1.0;
//        normalizedSamplingSize.height = cropScaleAmount.height/cropScaleAmount.width;
//    }
//    else {
//        normalizedSamplingSize.width = cropScaleAmount.width/cropScaleAmount.height;
//        normalizedSamplingSize.height = 1.0;;
//    }
//    
//    /*
//     The quad vertex data defines the region of 2D plane onto which we draw our pixel buffers.
//     Vertex data formed using (-1,-1) and (1,1) as the bottom left and top right coordinates respectively, covers the entire screen.
//     */
//    GLfloat quadVertexData [] = {
//        -1 * normalizedSamplingSize.width, -1 * normalizedSamplingSize.height,
//        normalizedSamplingSize.width, -1 * normalizedSamplingSize.height,
//        -1 * normalizedSamplingSize.width, normalizedSamplingSize.height,
//        normalizedSamplingSize.width, normalizedSamplingSize.height,
//    };
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    // Update attribute values.
    glEnableVertexAttribArray(ATTRIB_VERTEX);
    //glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, quadVertexData);
    glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, GL_FALSE, sizeof(GLVertex), (const GLvoid *) offsetof(GLVertex, Position));
    /*
     The texture vertices are set up such that we flip the texture vertically. This is so that our top left origin buffers match OpenGL's bottom left texture coordinate system.
     */
//    CGRect textureSamplingRect = CGRectMake(0, 0, 1, 1);
//    GLfloat quadTextureData[] =  {
//        CGRectGetMinX(textureSamplingRect), CGRectGetMaxY(textureSamplingRect),
//        CGRectGetMaxX(textureSamplingRect), CGRectGetMaxY(textureSamplingRect),
//        CGRectGetMinX(textureSamplingRect), CGRectGetMinY(textureSamplingRect),
//        CGRectGetMaxX(textureSamplingRect), CGRectGetMinY(textureSamplingRect)
//    };
    glEnableVertexAttribArray(ATTRIB_TEXCOORD);
    //glVertexAttribPointer(ATTRIB_TEXCOORD, 2, GL_FLOAT, 0, 0, quadTextureData);
    glVertexAttribPointer(ATTRIB_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(GLVertex),  (const GLvoid *) offsetof(GLVertex, TexCoord));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableVertexAttribArray(ATTRIB_VERTEX);
    glDisableVertexAttribArray(ATTRIB_TEXCOORD);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, _colorBufferHandle);
    [_context presentRenderbuffer:GL_RENDERBUFFER];

    [self cleanUpTextures];
    // Periodic texture cache flush every frame
    CVOpenGLESTextureCacheFlush(_videoTextureCache, 0);

    if(_videoTextureCache) {
        CFRelease(_videoTextureCache);
    }
    glUseProgram(0);
}

# pragma mark - OpenGL setup
- (void)setupGL
{
    if (!_context || ![EAGLContext setCurrentContext:_context])
    {
        return;
    }
    [self setupBuffers];
    [self loadShaders];
    glUseProgram(self.program);
    // 0 and 1 are the texture IDs of _lumaTexture and _chromaTexture respectively.
    glUniform1i(uniforms[UNIFORM_Y], 0);
    glUniform1i(uniforms[UNIFORM_UV], 1);

    int mWidth = 1920;
    int mHeight = 1080;

    float aspectRatio = ((float)1280/(float)720) / ((float)mWidth/(float)mHeight);
    NSLog(@"new width: %d, new height: %d", mWidth, mHeight);
    GLKMatrix4 modelViewProjectionMatrix = GLKMatrix4Scale(GLKMatrix4Identity, aspectRatio, -1, 1);
    GLKMatrix4 textureMatrix = GLKMatrix4Identity;
    glUniformMatrix4fv(uniforms[UNIFORM_MVPMATRIX], 1, false, modelViewProjectionMatrix.m);
    glUniformMatrix4fv(uniforms[UNIFORM_TEXMATRIX], 1, false, textureMatrix.m);
    glUniformMatrix3fv(uniforms[UNIFORM_COLOR_CONVERSION_MATRIX], 1, GL_FALSE, _preferredConversion);
    //glUniform1f(uniforms[UNIFORM_ROTATION_ANGLE], 0);
    glGenBuffers(1, &mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    if(_renderRotation == -90)
    {
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices_R90), Vertices_R90, GL_STATIC_DRAW);
    }
    else
    {
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

#pragma mark - Utilities

- (void)setupBuffers
{
    glDisable(GL_DEPTH_TEST);
    glEnableVertexAttribArray(ATTRIB_VERTEX);
    glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(ATTRIB_TEXCOORD);
    glVertexAttribPointer(ATTRIB_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
    [self createBuffers];
}

- (void) createBuffers
{
    glGenFramebuffers(1, &_frameBufferHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferHandle);
    glGenRenderbuffers(1, &_colorBufferHandle);
    glBindRenderbuffer(GL_RENDERBUFFER, _colorBufferHandle);
    [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:self];
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &_backingWidth);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &_backingHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _colorBufferHandle);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }
}

- (void) releaseBuffers
{
    if(_frameBufferHandle)
    {
        glDeleteFramebuffers(1, &_frameBufferHandle);
        _frameBufferHandle = 0;
    }
    if(_colorBufferHandle)
    {
        glDeleteRenderbuffers(1, &_colorBufferHandle);
        _colorBufferHandle = 0;
    }
}

- (void) resetRenderBuffer
{
    if (!_context || ![EAGLContext setCurrentContext:_context])
    {
        return;
    }
    [self releaseBuffers];
    [self createBuffers];
}

- (void) cleanUpTextures
{
    if (_lumaTexture)
    {
        CFRelease(_lumaTexture);
        _lumaTexture = NULL;
    }
    if (_chromaTexture)
    {
        CFRelease(_chromaTexture);
        _chromaTexture = NULL;
    }
}

#pragma mark -  OpenGL ES 2 shader compilation

//const GLchar *shader_fsh = (const GLchar*)"varying highp vec2 texCoordVarying;"
//"precision mediump float;"
//"uniform sampler2D SamplerY;"
//"uniform sampler2D SamplerUV;"
//"uniform mat3 colorConversionMatrix;"
//"void main()"
//"{"
//"    mediump vec3 yuv;"
//"    lowp vec3 rgb;"
////   Subtract constants to map the video range start at 0
//"    yuv.x = (texture2D(SamplerY, texCoordVarying).r - (16.0/255.0));"
//"    yuv.yz = (texture2D(SamplerUV, texCoordVarying).rg - vec2(0.5, 0.5));"
//"    rgb = colorConversionMatrix * yuv;"
//"    gl_FragColor = vec4(rgb, 1);"
//"}";
//
//const GLchar *shader_vsh = (const GLchar*)"attribute vec4 position;"
//"attribute vec2 texCoord;"
//"uniform float preferredRotation;"
//"varying vec2 texCoordVarying;"
//"void main()"
//"{"
//"    mat4 rotationMatrix = mat4(cos(preferredRotation), -sin(preferredRotation), 0.0, 0.0,"
//"                               sin(preferredRotation),  cos(preferredRotation), 0.0, 0.0,"
//"                               0.0,					    0.0, 1.0, 0.0,"
//"                               0.0,					    0.0, 0.0, 1.0);"
//"    gl_Position = position * rotationMatrix;"
//"    texCoordVarying = texCoord;"
//"}";

const GLchar *shader_fsh = (const GLchar*)"varying highp vec2 texCoordVarying;"
"precision mediump float;"
"uniform sampler2D SamplerY;"
"uniform sampler2D SamplerUV;"
"uniform mat3 colorConversionMatrix;"
"void main()"
"{"
"    mediump vec3 yuv;"
"    lowp vec3 rgb;"
//   Subtract constants to map the video range start at 0
"    yuv.x = (texture2D(SamplerY, texCoordVarying).r - (16.0/255.0));"
"    yuv.yz = (texture2D(SamplerUV, texCoordVarying).rg - vec2(0.5, 0.5));"
"    rgb = colorConversionMatrix * yuv;"
"    gl_FragColor = vec4(rgb, 1);"
"}";

const GLchar *shader_vsh = (const GLchar*)"attribute vec4 position;"
"attribute vec4 texCoord;"
"uniform mat4 texMatrix;"
"uniform mat4 mvpMatrix;"
"varying vec2 texCoordVarying;"
"void main()"
"{"
//"    mat4 rotationMatrix = mat4(cos(preferredRotation), -sin(preferredRotation), 0.0, 0.0,"
//"                               sin(preferredRotation),  cos(preferredRotation), 0.0, 0.0,"
//"                               0.0,					    0.0, 1.0, 0.0,"
//"                               0.0,					    0.0, 0.0, 1.0);"
//"    gl_Position = position * rotationMatrix;"
//"    texCoordVarying = texCoord;"
//"   texCoordVarying = (texMatrix * texCoord).xy;"
"   texCoordVarying = texCoord.xy;"
"   gl_Position = mvpMatrix * position;"

"}";

- (BOOL)loadShaders
{
    GLuint vertShader = 0, fragShader = 0;
    // Create the shader program.
    self.program = glCreateProgram();

    if(![self compileShaderString:&vertShader type:GL_VERTEX_SHADER shaderString:shader_vsh])
    {
        NSLog(@"Failed to compile vertex shader");
        return NO;
    }

    if(![self compileShaderString:&fragShader type:GL_FRAGMENT_SHADER shaderString:shader_fsh])
    {
        NSLog(@"Failed to compile fragment shader");
        return NO;
    }

    // Attach vertex shader to program.
    glAttachShader(self.program, vertShader);

    // Attach fragment shader to program.
    glAttachShader(self.program, fragShader);

    // Bind attribute locations. This needs to be done prior to linking.
    glBindAttribLocation(self.program, ATTRIB_VERTEX, "position");
    glBindAttribLocation(self.program, ATTRIB_TEXCOORD, "texCoord");

    // Link the program.
    if (![self linkProgram:self.program])
    {
        NSLog(@"Failed to link program: %d", self.program);

        if (vertShader)
        {
            glDeleteShader(vertShader);
            vertShader = 0;
        }
        if (fragShader)
        {
            glDeleteShader(fragShader);
            fragShader = 0;
        }
        if (self.program)
        {
            glDeleteProgram(self.program);
            self.program = 0;
        }
        return NO;
    }

    // Get uniform locations.
    uniforms[UNIFORM_Y] = glGetUniformLocation(self.program, "SamplerY");
    uniforms[UNIFORM_UV] = glGetUniformLocation(self.program, "SamplerUV");
    uniforms[UNIFORM_MVPMATRIX] = glGetUniformLocation(self.program, "texMatrix");
    uniforms[UNIFORM_TEXMATRIX] = glGetUniformLocation(self.program, "mvpMatrix");
    uniforms[UNIFORM_COLOR_CONVERSION_MATRIX] = glGetUniformLocation(self.program, "colorConversionMatrix");

    // Release vertex and fragment shaders.
    if (vertShader)
    {
        glDetachShader(self.program, vertShader);
        glDeleteShader(vertShader);
    }
    if (fragShader)
    {
        glDetachShader(self.program, fragShader);
        glDeleteShader(fragShader);
    }
    return YES;
}

- (BOOL)compileShaderString:(GLuint *)shader type:(GLenum)type shaderString:(const GLchar*)shaderString
{
    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, &shaderString, NULL);
    glCompileShader(*shader);

#if defined(DEBUG)
    GLint logLength;
    glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetShaderInfoLog(*shader, logLength, &logLength, log);
        NSLog(@"Shader compile log:\n%s", log);
        free(log);
    }
#endif

    GLint status = 0;
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
    if (status == 0)
    {
        glDeleteShader(*shader);
        return NO;
    }
    return YES;
}

- (BOOL)compileShader:(GLuint *)shader type:(GLenum)type URL:(NSURL *)URL
{
    NSError *error;
    NSString *sourceString = [[NSString alloc] initWithContentsOfURL:URL encoding:NSUTF8StringEncoding error:&error];
    if (sourceString == nil)
    {
        NSLog(@"Failed to load vertex shader: %@", [error localizedDescription]);
        return NO;
    }
    const GLchar *source = (GLchar *)[sourceString UTF8String];
    return [self compileShaderString:shader type:type shaderString:source];
}

- (BOOL)linkProgram:(GLuint)prog
{
    GLint status;
    glLinkProgram(prog);

#if defined(DEBUG)
    GLint logLength;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        NSLog(@"Program link log:\n%s", log);
        free(log);
    }
#endif

    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (status == 0)
    {
        return NO;
    }
    return YES;
}

- (BOOL)validateProgram:(GLuint)prog
{
    GLint logLength, status;

    glValidateProgram(prog);
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        NSLog(@"Program validate log:\n%s", log);
        free(log);
    }

    glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
    if (status == 0)
    {
        return NO;
    }
    return YES;
}

- (void)dealloc
{
    if (!_context || ![EAGLContext setCurrentContext:_context])
    {
        return;
    }

    [self cleanUpTextures];
    if (self.program)
    {
        glDeleteProgram(self.program);
        self.program = 0;
    }
    if(_context)
    {
        //[_context release];
        _context = nil;
    }
    //[super dealloc];
}

@end
