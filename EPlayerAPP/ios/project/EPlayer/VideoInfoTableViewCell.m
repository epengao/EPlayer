

#import "VideoInfoTableViewCell.h"
#import <AVFoundation/AVFoundation.h>

@interface VideoInfo ()
- (void)initThumbnailImage;
@end

@implementation VideoInfo
- (id)init
{
    self = [super init];
    if(self)
    {
        self.fileURL = nil;
        self.fileName = nil;
        self.fileType = nil;
        self.urlAsset = nil;
        self.fileSize = 0.0f;
        self.updateTime = nil;
        self.videoDuration = nil;
        self.videoCellType = iTunesVideoCell;
    }
    return self;
}

- (void)initThumbnailImage
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
    else{ /* TODO */}
    self.thumbnailImage = thumbnailImage;
}

- (UIImage *)getScreenShotImageFromVideoPath: (NSString*)filePath
{
    UIImage *retImage = nil;
    if(filePath != nil)
    {
        CGFloat startTime = 0.0;
        if(self.fileSize > 1.0)
        {
            startTime = 1.0f;
        }
        NSURL *fileURL = [NSURL fileURLWithPath:filePath];
        AVURLAsset *asset = [[AVURLAsset alloc] initWithURL:fileURL options:nil];
        CMTime time = CMTimeMakeWithSeconds(startTime, 600);
        retImage = [self getImageFromAVURLAsset:asset timeSpan:time];
    }
    return retImage;
}

- (UIImage *)getScreenShotImageFromAVURLAsset :(AVURLAsset*)urlAsset
{
    UIImage *retImage = nil;
    if(urlAsset != nil)
    {
        CGFloat startTime = 0.0;
        if(self.fileSize > 1.0)
        {
            startTime = 1.0f;
        }
        CMTime time = CMTimeMakeWithSeconds(startTime, 600);
        retImage = [self getImageFromAVURLAsset:urlAsset timeSpan:time];
    }
    return retImage;
}

- (UIImage*)getImageFromAVURLAsset :(AVURLAsset*)urlAsset timeSpan:(CMTime)time
{
    UIImage *retImage = nil;
    if(urlAsset != nil)
    {
        AVAssetImageGenerator *generator = [AVAssetImageGenerator assetImageGeneratorWithAsset:urlAsset];
        generator.appliesPreferredTrackTransform = YES;
        generator.maximumSize = CGSizeMake(1920, 1080);

        CMTime actualTime;
        NSError *error = nil;
        CGImageRef img = [generator copyCGImageAtTime:time actualTime:&actualTime error:&error];
        retImage = [[UIImage alloc] initWithCGImage: img];
    }
    return retImage;
}
@end

#define ThumbnailView_Tag   1000
#define FileNameView_Tag    1001
#define MediaMetadata_Tag   1002
#define MediaUpdateTime_Tag 1003

@interface VideoInfoTableViewCell ()
{
    VideoInfo *data;
}
@end

@implementation VideoInfoTableViewCell
- (void)awakeFromNib
{
    [super awakeFromNib];
}

- (void)setVideoInfo : (VideoInfo*)info
{
    if(info != nil)
    {
        if(data == nil)
        {
            data = [[VideoInfo alloc] init];
        }
        data.fileURL = info.fileURL;
        data.urlAsset = info.urlAsset;
        data.fileSize = info.fileSize;
        data.fileType = info.fileType;
        data.fileName = info.fileName;
        data.updateTime = info.updateTime;
        data.videoCellType = info.videoCellType;
        data.videoDuration = info.videoDuration;
        data.thumbnailImage = info.thumbnailImage;
    }
    if(data.thumbnailImage == nil)
    {
        [data initThumbnailImage];
    }
    info.thumbnailImage = data.thumbnailImage;
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated
{
    [super setSelected:selected animated:animated];
}

- (void)setFrame:(CGRect)frame
{
//    frame.origin.x = 5;
//    frame.size.width -= 2 * frame.origin.x;
//    frame.size.height -= frame.origin.x;
//    self.layer.masksToBounds = YES;

    self.layer.cornerRadius = 2.0;
    [super setFrame:frame];
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

- (void)configuVideoInfoCell;
{
    [self initThumbnail];
    [self initFileNameLableView];
    [self initVideoMetadataLableView];
    [self initUpdateTimeLableView];
}

- (void)initThumbnail
{
    [self createVideoThumbnailFromImage:data.thumbnailImage];
}

- (void)initFileNameLableView
{
    if(data.fileName != nil)
    {
        UILabel* fileNameLableView = [self viewWithTag:FileNameView_Tag];
        if(fileNameLableView == nil)
        {
            CGSize cellSize = self.frame.size;
            CGFloat targetWidth = cellSize.width * 0.5 - 8;
            CGFloat targetHeight = cellSize.height * 0.3;

            CGFloat y = 10;
            CGFloat x = targetWidth + 16;
            CGRect targetFrame = CGRectMake(x, y, targetWidth, targetHeight);

            fileNameLableView = [[UILabel alloc] initWithFrame:targetFrame];
            UIColor * color = [UIColor blackColor];
            [fileNameLableView setTag:FileNameView_Tag];
            [fileNameLableView setTextColor:color];
            [fileNameLableView setLineBreakMode:NSLineBreakByTruncatingMiddle];
            [fileNameLableView setFont:[UIFont fontWithName:@"Arial" size:14]];
            [self.contentView addSubview:fileNameLableView];
        }
        [fileNameLableView setText:data.fileName];
    }
}

- (void)initVideoMetadataLableView
{
    UILabel* metedataLableView = [self viewWithTag:MediaMetadata_Tag];
    if(metedataLableView == nil)
    {
        CGSize cellSize = self.frame.size;
        CGFloat targetWidth = cellSize.width * 0.5 - 8;
        CGFloat targetHeight = cellSize.height * 0.3;

        CGFloat y = targetHeight + 10;
        CGFloat x = targetWidth + 16;
        CGRect targetFrame = CGRectMake(x, y, targetWidth, targetHeight);
        metedataLableView = [[UILabel alloc] initWithFrame:targetFrame];

        UIColor * color = [UIColor colorWithRed:98.0f/256.0f green:98.0f/256.0f blue:98.0f/256.0f alpha:1.0];
        [metedataLableView setTag:MediaMetadata_Tag];

        [metedataLableView setTextColor:color];
        [metedataLableView setLineBreakMode:NSLineBreakByTruncatingMiddle];
        [metedataLableView setFont:[UIFont fontWithName:@"Arial" size:14]];
        [self.contentView addSubview:metedataLableView];
    }
    NSString *metadata = [NSString stringWithFormat:@"%@ | %@", data.videoDuration, data.fileType];
    [metedataLableView setText:metadata];
}

- (void)initUpdateTimeLableView
{
    UILabel* updateTimeLableView = [self viewWithTag:MediaUpdateTime_Tag];
    if(updateTimeLableView == nil)
    {
        CGSize cellSize = self.frame.size;
        CGFloat targetWidth = cellSize.width * 0.5 - 8;
        CGFloat targetHeight = cellSize.height * 0.3;

        CGFloat y = targetHeight * 2 + 10;
        CGFloat x = targetWidth + 16;
        CGRect targetFrame = CGRectMake(x, y, targetWidth, targetHeight);
        updateTimeLableView = [[UILabel alloc] initWithFrame:targetFrame];

        UIColor * color = [UIColor colorWithRed:98.0f/256.0f green:98.0f/256.0f blue:98.0f/256.0f alpha:1.0];
        [updateTimeLableView setTag:MediaUpdateTime_Tag];
        [updateTimeLableView setTextColor:color];
        [updateTimeLableView setLineBreakMode:NSLineBreakByTruncatingMiddle];
        [updateTimeLableView setFont:[UIFont fontWithName:@"Arial" size:14]];
        [self.contentView addSubview:updateTimeLableView];
    }
    NSString *updateTime = [NSString stringWithFormat:@"%@", data.updateTime];
    [updateTimeLableView setText:updateTime];
}

- (void)createVideoThumbnailFromImage :(UIImage*)thumbnailImage
{
    if(thumbnailImage == nil)
    {
        thumbnailImage = [UIImage imageNamed:@"default_video_thumbnail"];
    }
    UIImageView *thumbnailImageView = [self viewWithTag:ThumbnailView_Tag];
    if(thumbnailImageView == nil)
    {
        CGSize cellSize = self.frame.size;
        CGFloat targetWidth = cellSize.width * 0.5;
        CGFloat targetHeight = cellSize.height;

        CGFloat y = 0.0f;
        CGFloat x = 0.0f;
        CGRect targetFrame = CGRectMake(x, y, targetWidth, targetHeight);
        thumbnailImageView = [[UIImageView alloc] initWithFrame:targetFrame];
        thumbnailImageView.contentMode = UIViewContentModeScaleAspectFill;
        thumbnailImageView.image = thumbnailImage;
        [thumbnailImageView setTag:ThumbnailView_Tag];
        [self.contentView addSubview:thumbnailImageView];
        thumbnailImageView.layer.masksToBounds = YES;
        thumbnailImageView.layer.cornerRadius = 2.0;
    }
    [thumbnailImageView setImage:thumbnailImage];
}
@end
