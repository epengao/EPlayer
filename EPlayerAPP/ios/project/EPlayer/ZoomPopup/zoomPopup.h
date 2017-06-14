//
//  zoomPopup.h
//  TESTZOOMOUT
//
//  Created by Ingo Böhme on 24.12.13.
//  Copyright (c) 2013 AMOS. All rights reserved.
//
#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>


@interface zoomPopup : NSObject


@property (nonatomic) CGFloat borderSize;
@property (nonatomic, strong) UIColor* borderColor;
@property (nonatomic) CGFloat shadowRadius;
@property  (nonatomic) CGFloat backGroundAlpha;
@property  (nonatomic) BOOL showCloseButton;
@property  (nonatomic) NSInteger blurRadius;


+ (zoomPopup*) sharedInstance;


+(void) initWithMainview: (UIView*) mainView andStartRect: (CGRect) rect;
+(void) showPopup: (UIView*) popupView ;
+(void) closePopup;

+(void) setBorderSize: (CGFloat) borderSize;
+(void)  setBorderColor: ( UIColor*) borderColor;
+(void) setShadowRadius: ( CGFloat) shadowRadius;
+(void) setBackgroundAlpha: (CGFloat) backGroundAlpha;
+(void) showCloseButton: (BOOL) showCloseButton;


-(id) initWithMainview: (UIView*) mainView andStartRect: (CGRect) rect;
-(void) showPopup: (UIView*) popupView ;
-(void) closePopup;

@end
