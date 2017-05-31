//
//  WinTitleView.m
//  EPlayer
//
//  Created by GaoPeng on 2017/5/14.
//  Copyright © 2017年 EC. All rights reserved.
//

#import "WinTitleView.h"

@implementation WinTitleView

- (void)drawString :(NSString *)string inRect:(NSRect)rect {

    static NSDictionary * att = nil;
    if(!att){
        NSMutableParagraphStyle * style = [[NSParagraphStyle defaultParagraphStyle] mutableCopy];
        [style setLineBreakMode:NSLineBreakByTruncatingTail];
        [style setAlignment:NSTextAlignmentCenter];
        att = [[NSDictionary alloc] initWithObjectsAndKeys:style,NSParagraphStyleAttributeName,[NSColor whiteColor],NSForegroundColorAttributeName,[NSFont fontWithName:@"Helvetica"size:12],NSFontAttributeName,nil];
    }

    NSRect titlebarRect = NSMakeRect(rect.origin.x + 20,rect.origin.y-4,rect.size.width,rect.size.height);
    [string drawInRect:titlebarRect withAttributes:att];
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    NSRect windowFrame = [NSWindow  frameRectForContentRect:[[[self window] contentView] bounds] styleMask:[[self window] styleMask]];
    NSRect contentBounds = [[[self window] contentView] bounds];

    NSRect titlebarRect = NSMakeRect(0, 0, self.bounds.size.width, windowFrame.size.height - contentBounds.size.height);
    titlebarRect.origin.y = self.bounds.size.height - titlebarRect.size.height;

    NSRect topHalf, bottomHalf;
    NSDivideRect(titlebarRect, &topHalf, &bottomHalf, floor(titlebarRect.size.height/2.3), NSMaxYEdge);

    NSBezierPath * path = [NSBezierPath bezierPathWithRoundedRect:self.bounds xRadius:4.0 yRadius:4.0];
    [[NSBezierPath bezierPathWithRect:titlebarRect] addClip];

    NSGradient * gradient1 = [[NSGradient alloc] initWithStartingColor:[NSColor colorWithCalibratedWhite:0.10 alpha:1] endingColor:[NSColor colorWithCalibratedWhite:0.08 alpha:1]];
    NSGradient * gradient2 = [[NSGradient alloc] initWithStartingColor:[NSColor colorWithCalibratedWhite:0.10 alpha:1] endingColor:[NSColor colorWithCalibratedWhite:0.08 alpha:1]];
    [path addClip];

    // [[NSColor colorWithCalibratedWhite:0.00 alpha:1.0] set];
    // [path fill];

    [gradient1 drawInRect:topHalf angle:90.0];
    [gradient2 drawInRect:bottomHalf angle:270.0];
    [[NSColor blackColor] set];
    NSRectFill(NSMakeRect(0, -4, self.bounds.size.width, 1.0));

    [self drawString:@"EPlayer" inRect:titlebarRect];
}

- (CGFloat) getTitleHeight
{
    NSRect windowFrame = [NSWindow  frameRectForContentRect:[[[self window] contentView] bounds]
                                                  styleMask:[[self window] styleMask]];
    NSRect contentBounds = [[[self window] contentView] bounds];
    return windowFrame.size.height - contentBounds.size.height;
}

@end
