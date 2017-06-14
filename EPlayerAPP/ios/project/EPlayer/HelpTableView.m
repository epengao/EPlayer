//
//  HelpTableViewController.m
//  EPlayer
//
//  Created by Anthon Liu on 2017/6/14.
//  Copyright © 2017年 EC. All rights reserved.
//

#import "HelpTableView.h"


@interface HelpTableView ()
{
    NSInteger allSteps;
    NSMutableArray *allCells;
}
@end

@implementation HelpViewCell
- (void)setFrame:(CGRect)frame
{
    frame.origin.x = 5;
    frame.size.width -= 2 * frame.origin.x;
    frame.size.height -= frame.origin.x;
    self.layer.masksToBounds = YES;
    self.layer.cornerRadius = 4.0;
    
    [super setFrame:frame];
}
@end

@implementation HelpTableView

-(void)InitAllDataAndStyle {
    self.dataSource = self;
    self.sectionHeaderHeight = 0;
    [self setBackgroundColor:[UIColor colorWithRed:236.0f/256.0f green:236.0f/256.0f blue:236.0f/256.0f alpha:1.0]];
    self.separatorStyle = UITableViewCellSeparatorStyleNone;
    self.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self initCells];
}

- (void)initCells {
    allSteps = 0;
    if(self.type == MediaLibraryHelp){
        allSteps = [self initMediaLibraryHelpCells];
    }
    else if (self.type == UploadVideoHelp){
        [self initUploadVideoHelpCell];
    }
}

- (NSInteger)initMediaLibraryHelpCells {
    allCells = [NSMutableArray array];
    //NSArray *images = @[@"lib_help_step1", @"lib_help_step2", @"lib_help_step3"];
    NSArray *images = @[@"lib_help_step3"];
    for(NSString *imageName in images) {
        UITableViewCell *cell = [self createCellByImageName:imageName];
        if(cell != nil) {
            [allCells addObject:cell];
        }
    }
    return [allCells count];
};

- (void)initUploadVideoHelpCell {
    
}

-(UITableViewCell*)createCellByImageName :(NSString*)imageName {
    UITableViewCell *cell = nil;
    UIImage *image = [UIImage imageNamed:imageName];
    if(image != nil) {
        CGFloat y = 0.0f;
        CGFloat x = 0.0f;
        CGFloat targetWidth = self.frame.size.width;
        CGFloat targetHeight = image.size.height;
        CGRect targetFrame = CGRectMake(x, y, targetWidth, targetHeight);
        UIImageView *helpImageView = [[UIImageView alloc] initWithFrame:targetFrame];
        helpImageView.contentMode = UIViewContentModeScaleAspectFit;
        helpImageView.image = image;
        
        cell = [[UITableViewCell alloc] initWithFrame:targetFrame];
        [cell.contentView addSubview:helpImageView];
    }
    return cell;
}

#pragma mark - Table view data source
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return 1;
}
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    return [allCells objectAtIndex:indexPath.section];
}

-(CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPat {
    return 300;
}
@end
