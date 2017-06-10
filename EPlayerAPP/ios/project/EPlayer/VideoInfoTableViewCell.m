
#import "VideoInfoTableViewCell.h"
#import <AVFoundation/AVFoundation.h>

@implementation VideoInfoTableViewCell

- (void)awakeFromNib
{
    [super awakeFromNib];
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated
{
    [super setSelected:selected animated:animated];
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

- (void)configuVideoInfoCell;
{
    [self initThumbnail];
    [self initFileNameLableView];
}

- (void)setCellSelected : (BOOL)selected
{
    UIColor *selectedColor = nil;
    if(selected)
    {
        selectedColor = [UIColor colorWithRed:90.0f/256.0f green:53.0f/256.0f blue:200.0f/256.0f alpha:1.0];
    }
    else
    {
        selectedColor = [UIColor blackColor];
    }
    UILabel *lable = [self.contentView viewWithTag:1002];
    if(lable != nil)
    {
        [lable setTextColor:selectedColor];
    }
}

- (void)initFileNameLableView
{
    if(self.fileName != nil)
    {
        CGSize cellSize = self.frame.size;
        CGFloat targetWidth = cellSize.width * 0.5 - 8;
        CGFloat targetHeight = cellSize.height * 0.3;
        
        CGFloat y = targetHeight;
        CGFloat x = targetWidth + 16;
        CGRect targetFrame = CGRectMake(x, y, targetWidth, targetHeight);
        UILabel* fileNameLableView = [[UILabel alloc] initWithFrame:targetFrame];

        UIColor * color = [UIColor blackColor];
        [fileNameLableView setTag:1002];
        [fileNameLableView setText:self.fileName];
        [fileNameLableView setTextColor:color];
        [fileNameLableView setLineBreakMode:NSLineBreakByTruncatingMiddle];
        [fileNameLableView setFont:[UIFont fontWithName:@"Arial" size:14]];
        self.fileNameLable = fileNameLableView;
        [self.contentView addSubview:self.fileNameLable];
    }
}

- (void)initThumbnail
{
    UIImage *thumbnailImage = [self getScreenShotImageFromVideoPath:self.fileURL];
    if(thumbnailImage == nil)
    {
        thumbnailImage = [UIImage imageNamed:@"default_video_thumbnail"];
    }
    CGSize cellSize = self.frame.size;
    CGFloat targetWidth = cellSize.width * 0.5;
    CGFloat targetHeight = cellSize.height;
    
    CGFloat y = 0.0f;
    CGFloat x = 0.0f;
    CGRect targetFrame = CGRectMake(x, y, targetWidth, targetHeight);
    UIImageView* thumbnailImageView = [[UIImageView alloc] initWithFrame:targetFrame];
    thumbnailImageView.contentMode = UIViewContentModeScaleToFill;
    thumbnailImageView.image = thumbnailImage;
    
    UIView *subView = [[UIView alloc]initWithFrame:targetFrame];
    [subView addSubview:thumbnailImageView];
    self.videoThumbnailView = subView;
    [self.contentView addSubview:self.videoThumbnailView];
}

- (UIImage *)getScreenShotImageFromVideoPath:(NSString *)filePath
{
    if(filePath != nil)
    {
        UIImage *shotImage = nil;
        NSURL *fileURL = [NSURL fileURLWithPath:filePath];
        AVURLAsset *asset = [[AVURLAsset alloc] initWithURL:fileURL options:nil];
        AVAssetImageGenerator *gen = [[AVAssetImageGenerator alloc] initWithAsset:asset];
        gen.appliesPreferredTrackTransform = YES;
        CMTime time = CMTimeMakeWithSeconds(0.0, 600);

        NSError *error = nil;
        CMTime actualTime;
        CGImageRef image = [gen copyCGImageAtTime:time actualTime:&actualTime error:&error];
        shotImage = [[UIImage alloc] initWithCGImage:image];
        CGImageRelease(image);
        
        return shotImage;
    }
    return nil;
}
@end
