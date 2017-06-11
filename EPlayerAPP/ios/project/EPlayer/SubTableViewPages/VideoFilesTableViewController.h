//
//  WMTableViewController.h
//  Demo
//
//  Created by Mark on 16/7/25.
//  Copyright © 2016年 Wecan Studio. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef NS_ENUM(NSUInteger, VideoTableViewType)
{
    CameraVideosTableView     = 0,
    UploadVideosTableView     = 1,
    iTunesVideosTableView     = 2,
};

@interface VideoFilesTableViewController : UITableViewController
@property (nonatomic, assign) VideoTableViewType tableViewType;
- (void)setVideoFilesFolder :(NSString*)folderPath;
@end
