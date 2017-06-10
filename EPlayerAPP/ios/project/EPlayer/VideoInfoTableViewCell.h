//
//  VideoInfoTableViewCell.h
//  EPlayer
//
//  Created by GaoPeng on 2017/6/10.
//  Copyright © 2017年 EC. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface VideoInfoTableViewCell : UITableViewCell
@property (nonatomic, assign) CGFloat fileSize;
@property (nonatomic, assign) NSString *fileURL;
@property (nonatomic, assign) NSString *fileName;
@property (nonatomic, assign) NSString *fileFormat;
@property (nonatomic, assign) NSString *videoDuration;
@property (nonatomic, assign) UILabel *fileNameLable;
@property (nonatomic, assign) UILabel *metadataLable;
@property (nonatomic, assign) UIView *videoThumbnailView;

- (void)setCellSelected : (BOOL)selected;
- (void)configuVideoInfoCell;
@end
