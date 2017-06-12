//
//  VideoInfoTableViewCell.h
//  EPlayer
//
//  Created by GaoPeng on 2017/6/10.
//  Copyright © 2017年 EC. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <Photos/Photos.h>

typedef NS_ENUM(NSUInteger, VideoCellType)
{
    CameraVideoCell     = 0,
    UploadVideoCell     = 1,
    iTunesVideoCell     = 2,
};

@interface VideoInfo : NSObject
@property (nonatomic, copy) NSString *fileURL;
@property (nonatomic, copy) NSString *fileName;
@property (nonatomic, copy) NSString *fileType;
@property (nonatomic, copy) NSString *updateTime;
@property (nonatomic, copy) NSString *videoDuration;
@property (nonatomic, strong) AVURLAsset *urlAsset;
@property (nonatomic, strong) UIImage *thumbnailImage;
@property (nonatomic, assign) CGFloat fileSize;
@property (nonatomic, assign) VideoCellType videoCellType;
- (void)initThumbnailImage;
@end

@interface VideoInfoTableViewCell : UITableViewCell
- (void)setVideoInfo : (VideoInfo*)info;
- (void)setCellSelected : (BOOL)selected;
- (void)configuVideoInfoCell;
@end

