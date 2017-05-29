//
//  LADSlider.m
//  LADSliderExample
//
//  Created by Alexander Lapshin on 04.10.13.
//  Copyright (c) 2013 Alexander Lapshin. All rights reserved.
//

#import "LADSlider.h"
#import "LADSliderCell.h"

@interface NSImage (Rotated)
- (NSImage *)imageRotated:(CGFloat)degrees;
@end


@implementation LADSlider
//  We need to override it to prevent drawing bugs
//  Follow this link to know more about it:
//  http://stackoverflow.com/questions/3985816/custom-nsslidercell
- (void)setNeedsDisplayInRect:(NSRect)invalidRect {
    [super setNeedsDisplayInRect:[self bounds]];
}

- (void)awakeFromNib {
    [super awakeFromNib];
    
    if(![self.cell isKindOfClass:[LADSliderCell class]]) {
        LADSliderCell* myCell = [[LADSliderCell alloc] init];
        myCell.startDragAction = nil;
        myCell.doDraggingAction = nil;
        myCell.stopDragAction = nil;
        self.cell = myCell;
    }
}

- (id)initWithKnobImage:(NSImage *)knob {
    self = [super init];
    if (self) {
        self.cell = [[LADSliderCell alloc] initWithKnobImage:knob];
        return !self.cell ? nil : self;
    }

    return self;
}

- (id)initWithKnobImage:(NSImage *)knob minimumValueImage:(NSImage *)minImage maximumValueImage:(NSImage *)maxImage {
    self = [super init];
    if (self) {
        self.cell = [[LADSliderCell alloc] initWithKnobImage:knob minimumValueImage:minImage maximumValueImage:maxImage];
        return !self.cell ? nil : self;
    }

    return self;
}

- (LADSliderCell *)sliderCell {
	return self.cell;
}

- (NSImage *)knobImage {
    return self.sliderCell.knobImage;
}

- (void)setKnobImage:(NSImage *)image {
    self.sliderCell.knobImage = image;
}

- (NSImage *)minimumValueImage {
    return self.sliderCell.minimumValueImage;
}

- (void)setMinimumValueImage:(NSImage *)minimumValueImage {
    if (NSEdgeInsetsEqual(minimumValueImage.capInsets, NSEdgeInsetsZero)) {
        CGFloat inset = minimumValueImage.size.width-1;
        if (self.sliderCell.vertical) {
            minimumValueImage = [minimumValueImage imageRotated:90];
            minimumValueImage.capInsets = NSEdgeInsetsMake(0, 0, inset, 0);
        } else {
            minimumValueImage.capInsets = NSEdgeInsetsMake(0, inset, 0, 0);
        }
	}
	self.sliderCell.minimumValueImage = minimumValueImage;
}

- (NSImage *)maximumValueImage {
    return self.sliderCell.maximumValueImage;
}

- (void)setMaximumValueImage:(NSImage *)maximumValueImage {
    if (NSEdgeInsetsEqual(maximumValueImage.capInsets, NSEdgeInsetsZero)) {
        CGFloat inset = maximumValueImage.size.width-1;
        if (self.sliderCell.vertical) {
            maximumValueImage = [maximumValueImage imageRotated:90];
            maximumValueImage.capInsets = NSEdgeInsetsMake(inset, 0, 0, 0);
        } else {
            maximumValueImage.capInsets = NSEdgeInsetsMake(0, 0, 0, inset);
        }
	}
	self.sliderCell.maximumValueImage = maximumValueImage;
}

- (void)setTracingAction:(SEL)startDrag doDragging:(SEL)doDragging stopDrag:(SEL)stopDrag target:(id)target
{
    LADSliderCell *myCell = [self sliderCell];
    myCell.actionTarget = target;
    myCell.startDragAction = startDrag;
    myCell.doDraggingAction = doDragging;
    myCell.stopDragAction = stopDrag;
}

//- (void)mouseDown:(NSEvent *)theEvent
//{
//    [super mouseDown: theEvent];
//    NSLog(@"slider mouse down");
//}
//
//- (void)mouseUp: (NSEvent *)theEvent
//{
//    [super mouseUp: theEvent];
//    NSLog(@"slider mouse up");
//}
@end

@implementation NSImage (Rotated)

// Source from https://gist.github.com/Rm1210/10621763
- (NSImage *)imageRotated:(CGFloat)degrees {
    degrees = fmod(degrees, 360.);
    if (0 == degrees) {
        return self;
    }
    NSSize size = [self size];
    NSSize maxSize;
    if (90. == degrees || 270. == degrees || -90. == degrees || -270. == degrees) {
        maxSize = NSMakeSize(size.height, size.width);
    } else if (180. == degrees || -180. == degrees) {
        maxSize = size;
    } else {
        maxSize = NSMakeSize(20+MAX(size.width, size.height), 20+MAX(size.width, size.height));
    }
    NSAffineTransform *rot = [NSAffineTransform transform];
    [rot rotateByDegrees:degrees];
    NSAffineTransform *center = [NSAffineTransform transform];
    [center translateXBy:maxSize.width / 2. yBy:maxSize.height / 2.];
    [rot appendTransform:center];
    NSImage *image = [[NSImage alloc] initWithSize:maxSize];
    [image lockFocus];
    [rot concat];
    NSRect rect = NSMakeRect(0, 0, size.width, size.height);
    NSPoint corner = NSMakePoint(-size.width / 2., -size.height / 2.);
    [self drawAtPoint:corner fromRect:rect operation:NSCompositingOperationCopy fraction:1.0];
    [image unlockFocus];
    return image;
}
@end
