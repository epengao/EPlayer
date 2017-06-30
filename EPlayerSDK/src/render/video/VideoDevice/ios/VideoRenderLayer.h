
#import <GLKit/GLKit.h>
#include <CoreVideo/CoreVideo.h>
#include <QuartzCore/QuartzCore.h>

@interface VideoRenderLayer : CAEAGLLayer
- (id)initWithFrame:(CGRect)frame;
- (void)setDrawRect:(CGRect)rect;
- (void)resetRenderBuffer;
- (void)drawRGBBuffer:(uint8_t*)rgbBuffer;
- (void)drawPixelBuffer:(CVPixelBufferRef)pixelBuffer;
@end
