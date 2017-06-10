//
//  VideoInfoTableViewCell.m
//  EPlayer
//
//  Created by GaoPeng on 2017/6/10.
//  Copyright © 2017年 EC. All rights reserved.
//

#import "VideoInfoTableViewCell.h"

@implementation VideoInfoTableViewCell

- (void)awakeFromNib
{
    [super awakeFromNib];
    // Initialization code
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated
{
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

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
