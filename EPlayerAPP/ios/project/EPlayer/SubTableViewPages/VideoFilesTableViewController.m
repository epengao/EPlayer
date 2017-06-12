

#import "MBProgressHUD.h"
#import <Photos/Photos.h>
#import "VideoInfoTableViewCell.h"
#import "VideoFilesTableViewController.h"

@interface VideoFilesTableViewController ()
{
    BOOL loadingVideoInfo;
    NSInteger videoFilesCount;
    /* Camera Video files */
    PHFetchResult *assetsFetchResults;
    NSMutableArray *cameraVideoInfoList;
    /* Upload Video files */
    NSString* videoFileFolder;
    NSArray *uploadVideoInfoList;
}
@end

static NSString *const CameraTablewCellIdentifier = @"CameraTablewCellIdentifier";
@implementation VideoFilesTableViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    videoFilesCount = 0;
    self.tableView.dataSource = self;
    [[PHPhotoLibrary sharedPhotoLibrary] registerChangeObserver:self];
    [self.tableView setBackgroundColor:[UIColor colorWithRed:236.0f/256.0f green:236.0f/256.0f blue:236.0f/256.0f alpha:1.0]];
    self.tableView.separatorStyle = UITableViewCellSeparatorStyleNone;
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerClass:[VideoInfoTableViewCell class] forCellReuseIdentifier:CameraTablewCellIdentifier];

//    MBProgressHUD *hubAlertView = [[MBProgressHUD alloc] initWithView:self.view];
//    hubAlertView.removeFromSuperViewOnHide = YES;
//    hubAlertView.minSize = CGSizeMake(135.f, 135.f);
//    [self.view addSubview:hubAlertView];
//    
//    hubAlertView.labelText = NSLocalizedString(@"正在添加",nil);
//    [hubAlertView show:YES];
    loadingVideoInfo = YES;
    [self initAllVideoData];
    loadingVideoInfo = NO;
//    if([self getVideosCount] <= 0)
//    {
//        [self initEmptyTableBackground];
//    }
}

- (void)dealloc
{
    [PHPhotoLibrary.sharedPhotoLibrary unregisterChangeObserver:self];
}

- (void)setVideoFilesFolder :(NSString*)folderPath
{
    videoFileFolder = folderPath;
}

- (NSInteger)getVideosCount
{
    NSInteger count = 0;
    if(!loadingVideoInfo)
    {
        if(self.tableViewType == CameraVideosTableView)
        {
            count = [cameraVideoInfoList count];
        }
        else if(self.tableViewType == UploadVideosTableView)
        {
            count = [uploadVideoInfoList count];
        }
        else { /* TODO */}
    }
    return count;
}

- (NSInteger)initAllVideoData
{
    if(self.tableViewType == CameraVideosTableView)
    {
        return [self initCameraVideos];
    }
    else if(self.tableViewType == UploadVideosTableView)
    {
        return [self initUploadVideos];
    }
    else
    {
        return [self initItunesVideos];
    }
}

- (NSInteger)initCameraVideos
{
    NSInteger filesCount = 0;
    if(videoFileFolder != nil)
    {
        [self loadAllMediaLibraryVideos];
    }
    return filesCount;
}

- (NSInteger)initUploadVideos
{
    NSInteger filesCount = 0;
    if(videoFileFolder != nil)
    {
        uploadVideoInfoList = [[NSFileManager defaultManager] subpathsOfDirectoryAtPath:videoFileFolder error:nil];
        if(uploadVideoInfoList != nil)
        {
            filesCount = [uploadVideoInfoList count];
        }
    }
    return filesCount;
}

- (NSInteger)initItunesVideos
{
    return 0;
}

- (NSInteger)loadAllMediaLibraryVideos
{
    cameraVideoInfoList = [[NSMutableArray alloc] init];
    assetsFetchResults = [PHAssetCollection fetchAssetCollectionsWithType:PHAssetCollectionTypeSmartAlbum
                                                                  subtype:PHAssetCollectionSubtypeSmartAlbumVideos
                                                                  options:nil];
    for (PHAssetCollection *sub in assetsFetchResults)
    {
        PHFetchResult *assetsInCollection = [PHAsset fetchAssetsInAssetCollection:sub options:nil];
        for (PHAsset *asset in assetsInCollection)
        {
            NSArray *assetResources = [PHAssetResource assetResourcesForAsset:asset];
            for (PHAssetResource *assetRes in assetResources)
            {
                if (assetRes.type == PHAssetResourceTypeVideo ||
                    assetRes.type == PHAssetResourceTypeFullSizeVideo)
                {
                    PHVideoRequestOptions *videoFetchOptions = [PHVideoRequestOptions new];
                    videoFetchOptions.deliveryMode = PHVideoRequestOptionsDeliveryModeHighQualityFormat;
                    videoFetchOptions.networkAccessAllowed = NO;
                    __block AVURLAsset *urlAsset = nil;
                    dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
                    [[PHImageManager defaultManager] requestAVAssetForVideo:asset
                                                                    options:videoFetchOptions
                                                              resultHandler:^(AVAsset * _Nullable asset, AVAudioMix* _Nullable audioMix, NSDictionary* _Nullable info)
                     {
                         urlAsset = (AVURLAsset *)asset;
                         dispatch_semaphore_signal(semaphore);
                     }];
                    dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
                    if(urlAsset != nil)
                    {
                        [cameraVideoInfoList addObject:[self CreateCameraVideoInfoFromAVURLAsset:urlAsset needInitThumbnail:YES]];
                    }
                }
            }
        }
    }
    return [cameraVideoInfoList count];
}

- (void)initEmptyTableBackground
{
    UIImage* image = [UIImage imageNamed:@"no_video_bg"];
    CGSize tableSize = self.tableView.frame.size;
    CGFloat targetWidth = tableSize.width * 0.4;
    CGFloat targetHeight = (targetWidth * image.size.height) / image.size.width;

    CGFloat y = 70;
    CGFloat x = (tableSize.width - targetWidth) * 0.5;
    CGRect targetFrame = CGRectMake(x, y, targetWidth, targetHeight);
    UIImageView* emptyTableBackgroundImage = [[UIImageView alloc] initWithFrame:targetFrame];
    emptyTableBackgroundImage.contentMode = UIViewContentModeScaleToFill;
    emptyTableBackgroundImage.image = image;

    UIView *bgView = [[UIView alloc]initWithFrame:targetFrame];
    [bgView addSubview:emptyTableBackgroundImage];
    [self.tableView setBackgroundView:bgView];
}

#pragma mark - Config VideoInfo data
- (void)initVideoInfo :(VideoInfo*)videoInfo byFileURL:(NSString*)fileURL
{
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSDictionary *fileAttributes = [fileManager attributesOfItemAtPath:videoInfo.fileURL error:nil];
    if (fileAttributes != nil)
    {
        NSDate *updateDate = [fileAttributes objectForKey:NSFileModificationDate];
        NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
        [dateFormatter setDateFormat:@"yyyy-MM-dd HH:mm:ss"];
        videoInfo.updateTime = [dateFormatter stringFromDate:updateDate];
        videoInfo.fileSize = [[fileAttributes objectForKey:NSFileSize] floatValue] / (1000*1000.0f);
        
        NSURL *url = [NSURL fileURLWithPath:fileURL];
        AVURLAsset *urlAsset = [[AVURLAsset alloc] initWithURL:url options:nil];
        videoInfo.videoDuration = [self convertCMTimeToNSString:urlAsset.duration];
    }
}

- (VideoInfo*)CreateCameraVideoInfoFromAVURLAsset :(AVURLAsset*)urlAsset needInitThumbnail:(BOOL)needInitThumbnail
{
    VideoInfo *videoInfo = [[VideoInfo alloc] init];
    videoInfo.videoCellType = CameraVideoCell;
    if(urlAsset != nil)
    {
        videoInfo.fileURL = [urlAsset.URL absoluteString];
        videoInfo.fileName = [videoInfo.fileURL lastPathComponent];
        videoInfo.fileType = [videoInfo.fileURL pathExtension];
        if(urlAsset.creationDate != nil)
        {
            NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
            [dateFormatter setDateFormat:@"yyyy-MM-dd HH:mm:ss"];
            videoInfo.updateTime = [dateFormatter stringFromDate:urlAsset.creationDate.dateValue];
        }
        videoInfo.videoDuration = [self convertCMTimeToNSString:urlAsset.duration];
    }
    videoInfo.urlAsset = urlAsset;
    if(needInitThumbnail)
    {
        [videoInfo initThumbnailImage];
    }

    return videoInfo;
}

- (VideoInfo*)createUploadVideoInfoFromFileURL :(NSString*)url
{
    VideoInfo *videoInfo = [[VideoInfo alloc]init];
    videoInfo.videoCellType = UploadVideoCell;
    videoInfo.fileURL = url;
    videoInfo.fileName = [url lastPathComponent];
    videoInfo.fileType = [videoInfo.fileName pathExtension];
    [self initVideoInfo:videoInfo byFileURL:videoInfo.fileURL];
    [videoInfo initThumbnailImage];

    return videoInfo;
}

- (void)initItunesVideoInfo :(VideoInfo*)videoInfo
{
    videoInfo.videoCellType = iTunesVideoCell;
}

#pragma mark - Util
- (NSString*)convertCMTimeToNSString :(CMTime)time
{
    NSUInteger totalSeconds = CMTimeGetSeconds(time);
    NSUInteger hours = floor(totalSeconds / 3600);
    NSUInteger minutes = floor(totalSeconds % 3600 / 60);
    NSUInteger seconds = floor(totalSeconds % 3600 % 60);

    return [NSString stringWithFormat:@"%lu:%02lu:%02lu",hours, minutes, (unsigned long)seconds];
}

#pragma mark - Table view data source
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [self getVideosCount];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    VideoInfoTableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CameraTablewCellIdentifier];
    if (cell == nil)
    {
        cell = [[VideoInfoTableViewCell alloc] initWithStyle:UITableViewCellStyleDefault
                                             reuseIdentifier:CameraTablewCellIdentifier];
    }
    VideoInfo *info = nil;
    if(self.tableViewType == CameraVideosTableView)
    {
        info = cameraVideoInfoList[indexPath.row];
    }
    else if(self.tableViewType == UploadVideosTableView)
    {
        info = uploadVideoInfoList[indexPath.row];
    }
    else {/* TODO */}

    [cell setVideoInfo:info];
    [cell setFrame:CGRectMake(0, 0, self.view.frame.size.width - 10, 100)];
    [cell configuVideoInfoCell];
    return cell;
}

- (CGFloat)tableView:(UITableView*)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return 100;
}

- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section
{
    return 5;
}

- (CGFloat)tableView:(UITableView *)tableView heightForFooterInSection:(NSInteger)section
{
    return 0;
}

- (UIView *)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section
{
    UIView* view = [[UIView alloc] initWithFrame:CGRectMake(0, 0, self.tableView.frame.size.width, 5)];
    [view setBackgroundColor:[UIColor colorWithRed:236.0f/256.0f green:236.0f/256.0f blue:236.0f/256.0f alpha:1.0]];
    return view;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    //VideoInfoTableViewCell* cell = (VideoInfoTableViewCell*)[self tableView:tableView cellForRowAtIndexPath:indexPath];
    //if(cell != nil)
    //{
    //    [cell setCellSelected:YES];
    //}
    [tableView deselectRowAtIndexPath:indexPath animated:YES];
}

- (void)tableView:(UITableView *)tableView didDeselectRowAtIndexPath:(NSIndexPath *)indexPath
{
    //VideoInfoTableViewCell* cell = (VideoInfoTableViewCell*)[self tableView:tableView cellForRowAtIndexPath:indexPath];
    //if(cell != nil)
    //{
    //    [cell setCellSelected:NO];
    //}
}
- (BOOL)shouldAutorotate
{
    return YES;
}

- (UIInterfaceOrientationMask)supportedInterfaceOrientations
{
    return UIInterfaceOrientationMaskPortrait;
}

/* PHPhotoLibraryChangeObserver delegeate */
- (void)photoLibraryDidChange:(PHChange *)changeInstance
{
    PHFetchResultChangeDetails *collectionChanges = [changeInstance changeDetailsForFetchResult:assetsFetchResults];
    if (collectionChanges != nil)
    {
        if ([collectionChanges hasIncrementalChanges])
        {
            PHFetchResult *before = [collectionChanges fetchResultBeforeChanges];
            PHFetchResult *after = [collectionChanges fetchResultAfterChanges];
            /* Do diff and update data */
        }
    }
    dispatch_async(dispatch_get_main_queue(), ^
    {
        /* UI reload */
    });
}

@end
