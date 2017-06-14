//
//  zoomPopup.m
//  TESTZOOMOUT
//
//  Created by Ingo Böhme on 24.12.13.
//  Copyright (c) 2013 AMOS. All rights reserved.
//

#import "zoomPopup.h"
#import "UIImage+StackBlur.h"

@interface zoomPopup () {
	UIImageView *zoomImage;
	UIButton *closeButton;
    
	UIImageView *sourceImageView;
	UIImageView *destImageView;
    
    
	UIImageView *darkView;
	UIView *popupView;
    
	UIView *mainView;
    
    
	CGRect mainViewStartRect;
	
	BOOL isPresenting;
}


@end

@implementation zoomPopup

/**
 *  To use the class as a singleton
*/
+ (zoomPopup *)sharedInstance {
	static zoomPopup *_sharedInstance;
	if (!_sharedInstance) {
		static dispatch_once_t oncePredicate;
		dispatch_once(&oncePredicate, ^{
		    _sharedInstance = [[super allocWithZone:nil] init];
		});
	}
    
	return _sharedInstance;
}

+ (id)allocWithZone:(NSZone *)zone {
	return [self sharedInstance];
}

- (id)copyWithZone:(NSZone *)zone {
	return self;
}

/**
 *  Inits the Popup with the View it should be displayed in and the rect it should be animated from
 *
 *  @param underlayingView View it should be displayed
 *  @param rect            the rect it should be animated from
 */
+ (void)initWithMainview:(UIView *)underlayingView andStartRect:(CGRect)rect {
	id sender;
    sender = [[zoomPopup sharedInstance] initWithMainview:underlayingView andStartRect:rect];
}

- (id)initWithMainview:(UIView *)underlayingView andStartRect:(CGRect)rect {
	self = [super init];
	if (self) {
		mainView = underlayingView;
		mainViewStartRect = rect;
		_borderSize = 5;
		_borderColor = [UIColor colorWithRed:0 green:0 blue:0 alpha:0.8];
		_backGroundAlpha = 0.5;
		_shadowRadius = 10.0;
        _showCloseButton = YES;
        _blurRadius = 0;
	}
    
	return self;
}

/**
 *  Open the popup animated (Zoom and transition from one part of the screen
 *
 *  @param viewToDisplay The UIView (or descendant) that should be displayed
 */
+ (void)showPopup:(UIView *)viewToDisplay {
	[[zoomPopup sharedInstance] showPopup:viewToDisplay];
}

- (void)showPopup:(UIView *)viewToDisplay {
    
    if (!isPresenting) {
    
    	CGFloat width, height;
    
    	if (UIInterfaceOrientationIsPortrait([UIApplication sharedApplication].keyWindow.rootViewController.interfaceOrientation)) {
	 	width = MIN(mainView.frame.size.width, mainView.frame.size.height);
        	height = MAX(mainView.frame.size.width, mainView.frame.size.height);
    	} else {
		width = MAX(mainView.frame.size.width, mainView.frame.size.height);
        	height = MIN(mainView.frame.size.width, mainView.frame.size.height);
    	}
    
    	darkView = [[UIImageView alloc] initWithFrame:CGRectMake(0, 0, width , height)];
    	darkView.userInteractionEnabled = YES;
    	darkView.autoresizingMask = UIViewAutoresizingFlexibleHeight + UIViewAutoresizingFlexibleWidth;
    	if (_blurRadius == 0) {
        	darkView.backgroundColor = [UIColor blackColor];
    	} else {
        	darkView.image = [self imageWithView:mainView crop:mainView.bounds ];
        	darkView.image = [darkView.image stackBlur:_blurRadius];
    	}
    
	popupView = [[UIView alloc] initWithFrame:CGRectInset(viewToDisplay.frame, -_borderSize, -_borderSize)];
	popupView.backgroundColor = _borderColor;
	viewToDisplay.center = CGPointMake(viewToDisplay.bounds.size.width / 2 + _borderSize, viewToDisplay.bounds.size.height / 2 + _borderSize);
	[popupView addSubview:viewToDisplay];
    	popupView.autoresizingMask = UIViewAutoresizingFlexibleBottomMargin + UIViewAutoresizingFlexibleLeftMargin  + UIViewAutoresizingFlexibleRightMargin;
    	popupView.center = darkView.center;

	if (_showCloseButton) {
		closeButton = [UIButton buttonWithType:UIButtonTypeCustom];
		[closeButton addTarget:self action:@selector(closePopup) forControlEvents:UIControlEventTouchUpInside];
        
        
		UIImage *closeButtonImage = [UIImage imageNamed:@"zoomPopupClose"];
        if (closeButtonImage) {
            //CGSize closeButtonImageSize = CGSizeMake(closeButtonImage.size.width / [[UIScreen mainScreen] scale], closeButtonImage.size.height / [[UIScreen mainScreen] scale]);
            CGSize closeButtonImageSize = closeButtonImage.size;
            closeButton.bounds = CGRectMake(0, 0, closeButtonImageSize.width, closeButtonImageSize.height);
            [closeButton setImage:closeButtonImage forState:UIControlStateNormal];
            
            closeButton.center = CGPointMake(popupView.bounds.size.width - (closeButtonImageSize.width / 6),  (closeButtonImageSize.height / 6));
        } else {
            _showCloseButton = NO; // No zoomPopupClose@2x.png or zoomPopupClose.png => no close button
        }
	}
    
    
	UIImage *imgSrc = [self imageWithView:mainView crop:mainViewStartRect];
	UIImage *imgDest = [self imageWithView:popupView crop:popupView.bounds];
    
	sourceImageView = [[UIImageView alloc] initWithFrame:mainViewStartRect];
	destImageView = [[UIImageView alloc] initWithFrame:mainViewStartRect];
	[sourceImageView setImage:imgSrc];
	[destImageView setImage:imgDest];
	sourceImageView.alpha = 1.0;
	destImageView.alpha = 0.0;
	[mainView addSubview:darkView];
	[mainView addSubview:destImageView];
	[mainView addSubview:sourceImageView];
    
	darkView.alpha = 0;
	// darkView.blurRadius = 0;
	CFRunLoopRunInMode(CFRunLoopCopyCurrentMode(CFRunLoopGetCurrent()), 0, FALSE);
    
    
    
	[UIView animateWithDuration:0.5 delay:0 options:UIViewAnimationOptionCurveEaseInOut animations: ^(void) {
	    destImageView.alpha = 1.0;
	    destImageView.frame = popupView.frame;
	    sourceImageView.alpha = 0.0;
	    sourceImageView.frame = popupView.frame;
	    // darkView.blurRadius = _blur;
        if (_blurRadius == 0) {
            darkView.alpha = _backGroundAlpha;
        } else {
            darkView.alpha = 1;

        }
	}                completion: ^(BOOL finished) {
        sourceImageView.alpha = 0;
        destImageView.alpha = 0;
	    [mainView addSubview:popupView];
	    if (_showCloseButton) [popupView addSubview:closeButton];
	    popupView.layer.masksToBounds = NO;
	    popupView.layer.cornerRadius = 0; // if you like rounded corners
	    popupView.layer.shadowOffset = CGSizeMake(0, 0);
	    popupView.layer.shadowOpacity = 0.5;
	    popupView.layer.shadowRadius = _shadowRadius;
	}];
	
	isPresenting = TRUE;
    }
}

/**
 *  closes the Popup animated
 */
+ (void)closePopup {
	[[zoomPopup sharedInstance] closePopup];
}

- (void)closePopup {
	if (_showCloseButton) [closeButton removeFromSuperview];
	[popupView removeFromSuperview];
	[UIView animateWithDuration:0.3 delay:0 options:UIViewAnimationOptionCurveEaseInOut animations: ^(void) {
	    destImageView.alpha = 0.0;
	    destImageView.frame = mainViewStartRect;
	    sourceImageView.alpha = 1.0;
	    sourceImageView.frame = mainViewStartRect;
	    darkView.alpha = 0;
	}                completion: ^(BOOL finished) {
	    [destImageView removeFromSuperview];
	    [sourceImageView removeFromSuperview];
	    [darkView removeFromSuperview];
	}];
	isPresenting = FALSE;
}


+(void) setBorderSize: (CGFloat) borderSize {
    [zoomPopup sharedInstance].borderSize = borderSize;
}

+(void)  setBorderColor: ( UIColor*) borderColor{
    [zoomPopup sharedInstance].borderColor = borderColor;
}
+(void) setShadowRadius: ( CGFloat) shadowRadius{
    [zoomPopup sharedInstance].shadowRadius = shadowRadius;
}
+(void) setBackgroundAlpha: (CGFloat) backGroundAlpha{
    [zoomPopup sharedInstance].backGroundAlpha = backGroundAlpha;
}
+(void) setBlurRadius: (NSInteger) blurRadius{
    [zoomPopup sharedInstance].blurRadius = blurRadius;
}
+(void) showCloseButton: (BOOL) showCloseButton{
    [zoomPopup sharedInstance].showCloseButton = showCloseButton;
}



/**
 *  imageWithView   -  Get an UIImage of a view in the specified area (Screenshot)
 *
 *
 *  @param view the UIView to get the screen from
 *  @param rect area to be cropped
 *
 *  @return the UIImage of the area
 */
- (UIImage *)imageWithView:(UIView *)view crop:(CGRect)rect {
	UIGraphicsBeginImageContextWithOptions(view.bounds.size, view.opaque, 0.0);
	[view.layer renderInContext:UIGraphicsGetCurrentContext()];
    
	UIImage *img = UIGraphicsGetImageFromCurrentImageContext();
    
	UIGraphicsEndImageContext();
    
	rect = CGRectMake(rect.origin.x * img.scale,
	                  rect.origin.y * img.scale,
	                  rect.size.width * img.scale,
	                  rect.size.height * img.scale);
	CGImageRef imageRef = CGImageCreateWithImageInRect(img.CGImage, rect);
	UIImage *result = [UIImage imageWithCGImage:imageRef scale:img.scale orientation:img.imageOrientation];
	CGImageRelease(imageRef);
    
    
	return result;
}

@end
