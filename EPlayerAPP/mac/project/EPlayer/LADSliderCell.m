//
//  LADSliderCell.m
//  LADSliderExample
//
//  Created by Alexander Lapshin on 04.10.13.
//  Copyright (c) 2013 Alexander Lapshin. All rights reserved.
//

#import "LADSliderCell.h"

@interface LADSliderCell ()

@property (nonatomic) NSRect currentKnobRect;

@end

@implementation LADSliderCell

- (id)initWithKnobImage:(NSImage *)knob {
    self = [self init];
    if (self) {
        _knobImage = knob;
        return !knob ? nil : self;
    }

    return self;
}

- (id)initWithKnobImage:(NSImage *)knob minimumValueImage:(NSImage *)minImage maximumValueImage:(NSImage *)maxImage {
    self = [self init];
    if (self) {
        _knobImage = knob;
        self.minimumValueImage = minImage;
        self.maximumValueImage = maxImage;
        return (!knob || !minImage || !maxImage) ? nil : self;
    }

    return self;
}

- (BOOL)startTrackingAt:(NSPoint)startPoint inView:(NSView *)controlView {
    BOOL val = [super startTrackingAt:startPoint inView:controlView];
    [self drawInteriorWithFrame:controlView.bounds inView:controlView];
    return val;
}

- (void)drawKnob:(NSRect)knobRect {
    if (!_knobImage) {
        [super drawKnob:knobRect];
        return;
    }
    
    CGFloat dx = (knobRect.size.width - _knobImage.size.width) / 2.0;
    CGFloat dy = (knobRect.size.height - _knobImage.size.height) / 2.0;
    _currentKnobRect = CGRectInset(knobRect, dx, dy);
	
	[_knobImage drawInRect:_currentKnobRect];
}

- (void)drawBarInside:(NSRect)cellFrame flipped:(BOOL)flipped {
    if (!_knobImage || !_minimumValueImage || !_maximumValueImage) {
        [super drawBarInside:cellFrame flipped:flipped];
        return;
    }
    
	[_minimumValueImage drawInRect:[self beforeKnobRect:cellFrame]];
	[_maximumValueImage drawInRect:[self afterKnobRect:cellFrame]];
}

- (NSRect)beforeKnobRect:(NSRect)barRect {
    NSRect beforeKnobRect = barRect;
    NSSize minValueImageSize = _minimumValueImage.size;

    if (self.vertical) {
        beforeKnobRect.origin.x = CGRectGetMidX(barRect) - minValueImageSize.width / 2.0;
        beforeKnobRect.size.width = minValueImageSize.width;
        beforeKnobRect.size.height = CGRectGetMidY(_currentKnobRect) - barRect.origin.y;
    } else {
        beforeKnobRect.origin.y = CGRectGetMidY(barRect) - minValueImageSize.height / 2.0;
        beforeKnobRect.size.width = CGRectGetMidX(_currentKnobRect) - barRect.origin.x;
        beforeKnobRect.size.height = minValueImageSize.height;
    }
    
    return beforeKnobRect;
}

- (NSRect)afterKnobRect:(NSRect)barRect {
    NSRect afterKnobRect = barRect;
    NSSize maxValueImageSize = _maximumValueImage.size;
    
    if (self.vertical) {
        afterKnobRect.origin.x += (barRect.size.width - maxValueImageSize.width) / 2.0;
        afterKnobRect.origin.y = CGRectGetMidY(_currentKnobRect);
        afterKnobRect.size.width = maxValueImageSize.width;
        afterKnobRect.size.height -= CGRectGetMidY(_currentKnobRect);;
    } else {
        afterKnobRect.origin.x = CGRectGetMidX(_currentKnobRect);
        afterKnobRect.origin.y += (barRect.size.height - maxValueImageSize.height) / 2.0;
        afterKnobRect.size.width -= CGRectGetMidX(_currentKnobRect);
        afterKnobRect.size.height = maxValueImageSize.height;
    }
        
    return afterKnobRect;
}

@end


