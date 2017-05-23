//
//  PlayPausePanel.m
//  EPlayer
//
//  Created by Anthon Liu on 2017/5/16.
//  Copyright © 2017年 EC. All rights reserved.
//

#import "MediaCtrlViewShadow.h"

@implementation MediaCtrlViewShadow

- (void)drawRect:(NSRect)dirtyRect
{
    [super drawRect:dirtyRect];
    
    NSRect renderRect = NSMakeRect(0, 0, dirtyRect.size.width, dirtyRect.size.height);
    
    NSRect topHalf, bottomHalf;
    NSColor *startColor = [NSColor colorWithSRGBRed:0.1 green:0.1 blue:0.1 alpha:0.5];
    NSColor *endtColor = [NSColor colorWithSRGBRed:0.0 green:0.0 blue:0.0 alpha:0.5];

    NSDivideRect(renderRect, &topHalf, &bottomHalf, floor(renderRect.size.height/2.2), NSMaxYEdge);
    NSBezierPath * path = [NSBezierPath bezierPathWithRoundedRect:self.bounds xRadius:30.0 yRadius:30.0];
    [[NSBezierPath bezierPathWithRect:renderRect] addClip];
    
    NSGradient * gradient1 = [[NSGradient alloc] initWithStartingColor:startColor endingColor:endtColor];
    NSGradient * gradient2 = [[NSGradient alloc] initWithStartingColor:startColor endingColor:endtColor];
    [path addClip];
    
    [gradient1 drawInRect:topHalf angle:90.0];
    [gradient2 drawInRect:bottomHalf angle:270.0];
}
@end
