//
//  LADSlider.h
//  LADSliderExample
//
//  Created by Alexander Lapshin on 04.10.13.
//  Copyright (c) 2013 Alexander Lapshin. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface LADSlider : NSSlider

- (id)initWithKnobImage:(NSImage *)knob;
- (id)initWithKnobImage:(NSImage *)knob minimumValueImage:(NSImage *)minImage maximumValueImage:(NSImage *)maxImage;

@property (nonatomic) IBInspectable NSImage *knobImage;
@property (nonatomic) IBInspectable NSImage *minimumValueImage;
@property (nonatomic) IBInspectable NSImage *maximumValueImage;

@end
