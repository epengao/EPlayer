//
//  HelpTableViewController.h
//  EPlayer
//
//  Created by Anthon Liu on 2017/6/14.
//  Copyright © 2017年 EC. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef NS_ENUM(NSUInteger, HelpViewType)
{
    MediaLibraryHelp     = 0,
    UploadVideoHelp      = 1,
};

@interface HelpViewCell : UITableViewCell
@end

@interface HelpTableView : UITableView <UITableViewDataSource>
@property (nonatomic, assign) HelpViewType type;
-(void)InitAllDataAndStyle;
@end
