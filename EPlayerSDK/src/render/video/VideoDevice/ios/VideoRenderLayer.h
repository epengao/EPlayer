
#import <GLKit/GLKit.h>
#include <CoreVideo/CoreVideo.h>
#include <QuartzCore/QuartzCore.h>

@interface VideoRenderLayer : CAEAGLLayer
- (id)initWithFrame:(CGRect)frame;
- (void)setRenderFrame:(CGRect)frame;
- (void)resetRenderBuffer;
- (void)drawRGBBuffer:(uint8_t*)rgbBuffer;
- (void)setDrawRotation:(CGFloat)rotation;
- (void)drawPixelBuffer:(CVPixelBufferRef)pixelBuffer width:(CGFloat)width height:(CGFloat)height;
@end
