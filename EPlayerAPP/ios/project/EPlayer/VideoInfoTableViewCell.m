

#import "VideoInfoTableViewCell.h"
#import <AVFoundation/AVFoundation.h>

@interface VideoInfoTableViewCell ()
@property (nonatomic, weak) UILabel *fileNameLable;
@property (nonatomic, weak) UILabel *metadataLable;
@property (nonatomic, weak) UILabel *updateTimeLable;
@property (nonatomic, weak) UIView *videoThumbnailView;
@end

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
    [self initVideoMetadataLableView];
    [self initUpdateTimeLableView];
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
        
        CGFloat y = 10;
        CGFloat x = targetWidth + 16;
        CGRect targetFrame = CGRectMake(x, y, targetWidth, targetHeight);
        UILabel* fileNameLableView = [[UILabel alloc] initWithFrame:targetFrame];

        UIColor * color = [UIColor blackColor];
        [fileNameLableView setTag:1001];
        [fileNameLableView setText:self.fileName];
        [fileNameLableView setTextColor:color];
        [fileNameLableView setLineBreakMode:NSLineBreakByTruncatingMiddle];
        [fileNameLableView setFont:[UIFont fontWithName:@"Arial" size:14]];
        self.fileNameLable = fileNameLableView;
        [self.contentView addSubview:self.fileNameLable];
    }
}

- (void)initVideoMetadataLableView
{
    CGSize cellSize = self.frame.size;
    CGFloat targetWidth = cellSize.width * 0.5 - 8;
    CGFloat targetHeight = cellSize.height * 0.3;
    
    CGFloat y = targetHeight + 10;
    CGFloat x = targetWidth + 16;
    CGRect targetFrame = CGRectMake(x, y, targetWidth, targetHeight);
    UILabel* metedataLableView = [[UILabel alloc] initWithFrame:targetFrame];
    
    NSString *metadata = [NSString stringWithFormat:@"%.2fM | %@", _fileSize, _fileType];
    
    UIColor * color = [UIColor colorWithRed:98.0f/256.0f green:98.0f/256.0f blue:98.0f/256.0f alpha:1.0];
    [metedataLableView setTag:1002];
    [metedataLableView setText:metadata];
    [metedataLableView setTextColor:color];
    [metedataLableView setLineBreakMode:NSLineBreakByTruncatingMiddle];
    [metedataLableView setFont:[UIFont fontWithName:@"Arial" size:14]];
    self.metadataLable = metedataLableView;
    [self.contentView addSubview:self.metadataLable];
}

- (void)initUpdateTimeLableView
{
    CGSize cellSize = self.frame.size;
    CGFloat targetWidth = cellSize.width * 0.5 - 8;
    CGFloat targetHeight = cellSize.height * 0.3;
    
    CGFloat y = targetHeight * 2 + 10;
    CGFloat x = targetWidth + 16;
    CGRect targetFrame = CGRectMake(x, y, targetWidth, targetHeight);
    UILabel* updateTimeLableView = [[UILabel alloc] initWithFrame:targetFrame];
    
    NSString *updateTime = [NSString stringWithFormat:@"%@", _updateTime];
    
    UIColor * color = [UIColor colorWithRed:98.0f/256.0f green:98.0f/256.0f blue:98.0f/256.0f alpha:1.0];
    [updateTimeLableView setTag:1003];
    [updateTimeLableView setText:updateTime];
    [updateTimeLableView setTextColor:color];
    [updateTimeLableView setLineBreakMode:NSLineBreakByTruncatingMiddle];
    [updateTimeLableView setFont:[UIFont fontWithName:@"Arial" size:14]];
    self.updateTimeLable = updateTimeLableView;
    [self.contentView addSubview:self.updateTimeLable];
}

- (UIImage *)getScreenShotImageFromAVURLAsset :(AVURLAsset*)urlAsset
{
    if(urlAsset != nil)
    {
        AVAssetImageGenerator *generator = [AVAssetImageGenerator assetImageGeneratorWithAsset:urlAsset];
        generator.appliesPreferredTrackTransform = YES;
        generator.maximumSize = CGSizeMake(1920, 1080);
        NSError *error = nil;
        CGImageRef img = [generator copyCGImageAtTime:CMTimeMake(0, 10000) actualTime:NULL error:&error];
        UIImage *image = [UIImage imageWithCGImage: img];
        return image;
    }
    return nil;
}

- (UIImage *)getScreenShotImageFromVideoPath:(NSString *)filePath
{
    if(filePath != nil)
    {
        CGFloat startTime = 0.0;
        if(self.fileSize > 1.0)
        {
            startTime = 1.0f;
        }
        UIImage *shotImage = nil;
        NSURL *fileURL = [NSURL fileURLWithPath:filePath];
        AVURLAsset *asset = [[AVURLAsset alloc] initWithURL:fileURL options:nil];
        AVAssetImageGenerator *gen = [[AVAssetImageGenerator alloc] initWithAsset:asset];
        gen.appliesPreferredTrackTransform = YES;
        CMTime time = CMTimeMakeWithSeconds(startTime, 600);

        NSError *error = nil;
        CMTime actualTime;
        CGImageRef image = [gen copyCGImageAtTime:time actualTime:&actualTime error:&error];
        shotImage = [[UIImage alloc] initWithCGImage:image];
        CGImageRelease(image);
        return shotImage;
    }
    return nil;
}

- (void)createVideoThumbnailFromImage :(UIImage*)thumbnailImage
{
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
    thumbnailImageView.contentMode = UIViewContentModeScaleAspectFill;
    thumbnailImageView.image = thumbnailImage;
    
    UIView *subView = [[UIView alloc]initWithFrame:targetFrame];
    [subView addSubview:thumbnailImageView];
    [subView setTag:1000];
    self.videoThumbnailView = subView;
    [self.contentView addSubview:self.videoThumbnailView];
}

- (void)initThumbnail
{
    UIImage *thumbnailImage = nil;
    if(self.videoCellType == CameraVideoCell)
    {
        thumbnailImage = [self getScreenShotImageFromAVURLAsset:self.urlAsset];
    }
    else if (self.videoCellType == UploadVideoCell)
    {
        thumbnailImage = [self getScreenShotImageFromVideoPath:self.fileURL];
    }
    else
    {
    }
    [self createVideoThumbnailFromImage:thumbnailImage];
}

@end
