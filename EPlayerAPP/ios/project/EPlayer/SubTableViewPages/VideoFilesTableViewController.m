
#import "DXAlertView.h"
#import <Photos/Photos.h>
#import "MBProgressHUD.h"
#import "VideoInfoTableViewCell.h"
#import "PlayVideoViewController.h"
#import "VideoFilesTableViewController.h"

@interface VideoFilesTableViewController ()
{
    NSInteger videoFilesCount;
    NSMutableArray *videoInfoList;
    /* Camera Video files */
    PHFetchResult *assetsFetchResults;
    //NSMutableArray *cameraVideoInfoList;
    /* Upload Video files */
    NSString* videoFileFolder;
    NSArray *uploadVideoFilesList;
    //NSMutableArray *uploadVideoInfoList;
}
@property (nonatomic, strong) UIButton *helpButton;
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

    NSThread *loadVideoInfoThread = [[NSThread alloc]initWithTarget:self selector:@selector(initAllVideoData) object:nil];
    [loadVideoInfoThread start];
}

- (void)dealloc
{
    [PHPhotoLibrary.sharedPhotoLibrary unregisterChangeObserver:self];
}

- (void)setVideoFilesFolder :(NSString*)folderPath
{
    videoFileFolder = folderPath;
}

- (void)reloadAllVideosInfo
{
    NSThread *loadVideoInfoThread = [[NSThread alloc]initWithTarget:self selector:@selector(initAllVideoData) object:nil];
    [loadVideoInfoThread start];
}

- (void)reloadTableView
{
    [self.tableView reloadData];
}

- (void)updateTableViewUIAtMainThread
{
    [self performSelectorOnMainThread:@selector(reloadTableView) withObject:nil waitUntilDone:YES];
}

- (void)setNoMediaLibraryAuthorization
{
    [videoInfoList removeAllObjects];
    [self setEmptyTableBackground];
    [self addSetMediaAccessAuthoriztionHelpButton];
    [self updateTableViewUIAtMainThread];
}

- (void)addSetMediaAccessAuthoriztionHelpButton;
{
    NSString *buttonTitle = NSLocalizedString(@"获取相册权限",nil);
    [self createHelpButtonWithTitle:buttonTitle];
}

- (void)addUploadVideoFileToPhoneHelpButton;
{
    NSString *buttonTitle = NSLocalizedString(@"从电脑导入视频",nil);
    [self createHelpButtonWithTitle:buttonTitle];
}

- (void)createHelpButtonWithTitle: (NSString*)title
{
    CGFloat targetWidth = 130;
    CGFloat targetHeight = 26;
    CGFloat x = (self.tableView.frame.size.width - targetWidth) * 0.5;
    CGFloat y = (self.tableView.frame.size.width - targetHeight) *0.5 + 5;
    CGRect targetFrame = CGRectMake(x, y, targetWidth, targetHeight);
    
    if(_helpButton == nil)
    {
        _helpButton = [[UIButton alloc]initWithFrame:targetFrame];
    }
    UIColor *normalColor = [UIColor colorWithRed:0 green:0 blue:1 alpha:0.6];
    UIColor *highlightedColor = [UIColor colorWithRed:(121.f/256.f) green:(121.f/256.f) blue:(121.f/256.f) alpha:1.0];
    [_helpButton addTarget:self action:@selector(helpButtonClicked) forControlEvents:UIControlEventTouchUpInside];
    [_helpButton setTitleColor:normalColor forState:UIControlStateNormal];
    [_helpButton setTitleColor:highlightedColor forState:UIControlStateHighlighted];
    
    NSRange titleRange = {0,[title length]};
    NSMutableAttributedString *titleStr = [[NSMutableAttributedString alloc] initWithString:title];
    [titleStr addAttribute:NSForegroundColorAttributeName value:normalColor range:titleRange];
    [titleStr addAttribute:NSUnderlineStyleAttributeName value:[NSNumber numberWithInteger:NSUnderlineStyleSingle] range:titleRange];
    [_helpButton setAttributedTitle:titleStr forState:UIControlStateNormal];
    
    NSRange titleRange0 = {0,[title length]};
    NSMutableAttributedString *titleStr0 = [[NSMutableAttributedString alloc] initWithString:title];
    [titleStr0 addAttribute:NSForegroundColorAttributeName value:highlightedColor range:titleRange0];
    [titleStr0 addAttribute:NSUnderlineStyleAttributeName value:[NSNumber numberWithInteger:NSUnderlineStyleSingle] range:titleRange];
    [_helpButton setAttributedTitle:titleStr0 forState:UIControlStateHighlighted];
    
    [self.tableView.backgroundView addSubview:_helpButton];
}

- (void)helpButtonClicked
{
    if(self.tableViewType == CameraVideosTableView)
        [self showAcquireAuthorizationHelp];
    else if(self.tableViewType == UploadVideosTableView)
        [self showUploadVideoToPhoneHelp];
}

- (NSInteger)getVideosCount
{
    return [videoInfoList count];
}

- (NSInteger)initAllVideoData
{
    NSInteger ret = 0;
    if(self.tableViewType == CameraVideosTableView)
    {
        ret = [self initCameraVideos];
    }
    else if(self.tableViewType == UploadVideosTableView)
    {
        ret = [self initUploadVideos];
    }
    else
    {
        ret = [self initItunesVideos];
    }
    [self.tableView reloadData];
    return ret;
}

- (NSInteger)initCameraVideos
{
//    MBProgressHUD *hubAlertView = [[MBProgressHUD alloc] initWithView:self.view];
//    hubAlertView.removeFromSuperViewOnHide = YES;
//    hubAlertView.minSize = CGSizeMake(60.f, 60.f);
//    [self.view addSubview:hubAlertView];
//    hubAlertView.labelText = NSLocalizedString(@"正在加载",nil);
//    [hubAlertView show:YES];

    NSInteger videosCount = 0;
    NSString *finishedMsg = nil;
    if(videoFileFolder != nil)
    {
        [self loadAllMediaLibraryVideos];
        videosCount = [videoInfoList count];
        finishedMsg = NSLocalizedString(@"加载完毕", nil);
    }
    else
    {
        finishedMsg = NSLocalizedString(@"无相册访问权限", nil);
    }
    
    if(videosCount <= 0)
    {
        if(self.accessMeidaRight == CanAccess)
        {
            [self setEmptyTableBackground];
        }
        else if (self.accessMeidaRight == CanNotAccess)
        {
            [self setEmptyTableBackground];
            [self addSetMediaAccessAuthoriztionHelpButton];
        }
    }

//    MBProgressHUD *hud = [MBProgressHUD HUDForView:self.view];
//    hud.mode = MBProgressHUDModeAnnularDeterminate;
//    hud.labelText = finishedMsg;
//    [MBProgressHUD hideAllHUDsForView:self.view animated:YES];

    return videosCount;
}

- (NSInteger)initUploadVideos
{
    NSInteger filesCount = 0;
    if(videoFileFolder != nil)
    {
        uploadVideoFilesList = [[NSFileManager defaultManager] subpathsOfDirectoryAtPath:videoFileFolder error:nil];
        if(uploadVideoFilesList != nil)
        {
            videoInfoList = [NSMutableArray array];
            for(NSString *fileName in uploadVideoFilesList)
            {
                NSString *fileURL = [NSString stringWithFormat:@"%@/%@", videoFileFolder, fileName];
                [videoInfoList addObject:[self createUploadVideoInfoFromFileURL:fileURL]];
            }
        }
        filesCount = [videoInfoList count];
    }
    if(filesCount <= 0)
    {
        [self setEmptyTableBackground];
        [self addUploadVideoFileToPhoneHelpButton];
    }
    return filesCount;
}

- (NSInteger)initItunesVideos
{
    [self setEmptyTableBackground];
    return 0;
}

- (NSInteger)loadAllMediaLibraryVideos
{
    videoInfoList = [[NSMutableArray alloc] init];
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
                        [videoInfoList addObject:[self CreateCameraVideoInfoFromAVURLAsset:urlAsset needInitThumbnail:YES]];
                    }
                    [self updateTableViewUIAtMainThread];
                }
            }
        }
    }
    return [videoInfoList count];
}

- (void)setEmptyTableBackground
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
    if( (self.tableViewType == CameraVideosTableView) ||
        (self.tableViewType == UploadVideosTableView) )
    {
        info = videoInfoList[indexPath.row];
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
    [view setBackgroundColor:[UIColor colorWithRed:236.0f/256.0f green:236.0f/256.0f blue:236.0f/256.0f alpha:0.0]];
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
    VideoInfo *info = nil;
    if( (self.tableViewType == CameraVideosTableView) ||
       (self.tableViewType == UploadVideosTableView) )
    {
        info = videoInfoList[indexPath.row];
    }
    else {/* TODO */}
    PlayVideoViewController *playView = [[PlayVideoViewController alloc]init];
    if(info != nil)
    {
        playView.videoFileURL = info.fileURL;
        [self presentViewController:playView animated:YES completion:nil];
    }
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

#pragma mark - Help Views
- (void)showAcquireAuthorizationHelp
{
    DXAlertView *alert = [[DXAlertView alloc] initWithTitle:@"打开相册权限" contentText:@"设置->隐私->照片->EPlayer \n打开访问权限" leftButtonTitle:nil rightButtonTitle:@"确定"];
    [alert show];
    alert.rightBlock = ^() {
        NSLog(@"right button clicked");
    };
    alert.dismissBlock = ^() {
        NSLog(@"Do something interesting after dismiss block");
    };
}

- (void)showUploadVideoToPhoneHelp
{
    DXAlertView *alert = [[DXAlertView alloc] initWithTitle:@"视频导入手机" contentText:@"通过手机连接电脑，打开iTunes通过文件共享拷贝到手机" leftButtonTitle:nil rightButtonTitle:@"确定"];
    [alert show];
    alert.rightBlock = ^() {
        NSLog(@"right button clicked");
    };
    alert.dismissBlock = ^() {
        NSLog(@"Do something interesting after dismiss block");
    };
}
@end
