//
//  FullScreenViewController.h
//  Tapestry
//
//  Created by Kortide on 15/8/6.
//  Copyright (c) 2015年 Kortide. All rights reserved.
//

#import <UIKit/UIKit.h>

@class VideoFilesTableViewController;

@interface PlayVideoViewController : UIViewController
@property (nonatomic, strong) NSString *videoFileURL;
@property (nonatomic, assign) VideoFilesTableViewController *parent;
@end
