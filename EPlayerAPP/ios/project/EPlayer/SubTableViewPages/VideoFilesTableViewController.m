
#import "JRToast.h"
#import "DXAlertView.h"
#import <Photos/Photos.h>
#import "MBProgressHUD.h"
#import "YiRefreshHeader.h"
#import "YiRefreshFooter.h"
#import "VideoInfoTableViewCell.h"
#import "PlayVideoViewController.h"
#import "VideoFilesTableViewController.h"

@interface VideoFilesTableViewController ()
{
    NSInteger videoFilesCount;
    NSString* videoFileFolder;
    NSMutableArray *videoInfoList;
    NSMutableArray *videoInfoListUpdate;
    NSMutableArray *videoinfoListSwitch;
    /* Camera Video files use it*/
    PHFetchResult *assetsFetchResults;
    /* Refresh control */
    YiRefreshHeader *refreshHeader;
    YiRefreshFooter *refreshFooter;
    /* Update list Flag */
    BOOL doingUpdate;
}
@property (nonatomic, strong) UIButton *helpButton;
@end

static NSString *const CameraTablewCellIdentifier = @"CameraTablewCellIdentifier";
@implementation VideoFilesTableViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    doingUpdate = NO;
    videoFilesCount = 0;
    [self initRefreshController];
    self.tableView.dataSource = self;
    videoInfoList = [[NSMutableArray alloc] init];
    videoInfoListUpdate = [[NSMutableArray alloc] init];
    videoinfoListSwitch = [[NSMutableArray alloc] init];
    //We use YiRefresh to update the new Photo videos, no need regest
    //[[PHPhotoLibrary sharedPhotoLibrary] registerChangeObserver:self];
    [self.tableView setBackgroundColor:[UIColor colorWithRed:236.0f/256.0f green:236.0f/256.0f blue:236.0f/256.0f alpha:1.0]];
    self.tableView.separatorStyle = UITableViewCellSeparatorStyleNone;
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerClass:[VideoInfoTableViewCell class] forCellReuseIdentifier:CameraTablewCellIdentifier];

    NSThread *loadVideoInfoThread = [[NSThread alloc]initWithTarget:self selector:@selector(initVideoData) object:nil];
    [loadVideoInfoThread start];
}

//We use YiRefresh to update the new Photo videos, no need regest
//- (void)dealloc
//{
//    [PHPhotoLibrary.sharedPhotoLibrary unregisterChangeObserver:self];
//}

- (void)initRefreshController
{
    if ([[[UIDevice currentDevice]systemVersion] floatValue] >= 7.0)
    {
        self.edgesForExtendedLayout = UIRectEdgeBottom | UIRectEdgeLeft | UIRectEdgeRight;
    }
    self.automaticallyAdjustsScrollViewInsets=NO;

    __block NSString *noticeStr = nil;
    if(self.tableViewType == CameraVideosTableView)
    {
        noticeStr = @"相机胶卷中 ";
    }
    else if(self.tableViewType == UploadVideosTableView)
    {
        noticeStr = @"EPlayer文件夹中 ";
    }
    else
    {
        noticeStr = @"刷新 ";
    }

    refreshHeader=[[YiRefreshHeader alloc] init];
    refreshHeader.scrollView = self.tableView;
    [refreshHeader header];
    typeof(refreshHeader) __weak weakRefreshHeader = refreshHeader;
    refreshHeader.beginRefreshingBlock=^(){
        dispatch_async(dispatch_get_global_queue(0, 0), ^{
            if(doingUpdate == NO)
            {
                doingUpdate = YES;
                [self doPreUpdate];
                sleep(1.0);
                dispatch_async(dispatch_get_main_queue(), ^{
                    typeof(weakRefreshHeader) __strong strongRefreshHeader = weakRefreshHeader;
                    [self doPostUpdate];
                    [self.tableView reloadData];
                    doingUpdate = NO;
                    NSUInteger videoCount = [self getVideosCount];
                    CGFloat topOffset = [self.mainVC getTopViewHeight];
                    NSString *notice = [NSString stringWithFormat:@"%@ %ld 个视频", noticeStr, videoCount];
                    [JRToast showWithText:notice topOffset:topOffset duration:0.8f];
                    [self checkBackgroundSetting];
                    [strongRefreshHeader endRefreshing];
                });
            }
        });
    };
    //[refreshHeader beginRefreshing];

    /*
    refreshFooter=[[YiRefreshFooter alloc] init];
    refreshFooter.scrollView = self.tableView;
    [refreshFooter footer];
    typeof(refreshFooter) __weak weakRefreshFooter = refreshFooter;
    refreshFooter.beginRefreshingBlock=^(){
        dispatch_async(dispatch_get_global_queue(0, 0), ^{
            sleep(2);
            dispatch_async(dispatch_get_main_queue(), ^{
                typeof(weakRefreshFooter) __strong strongRefreshFooter = weakRefreshFooter;
                NSLog(@"foot refreshed");
                [strongRefreshFooter endRefreshing];
            });
        });
    };
    */
}

- (void)setVideoFilesFolder :(NSString*)folderPath
{
    videoFileFolder = folderPath;
}

- (void)clearAllVideoInfo
{
    [videoInfoList removeAllObjects];
}

- (void)reloadAllVideosInfo
{
    NSThread *loadVideoInfoThread = [[NSThread alloc]initWithTarget:self selector:@selector(initVideoData) object:nil];
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
    [self checkBackgroundSetting];
    [self addSetMediaAccessAuthoriztionHelpButton];
    [self updateTableViewUIAtMainThread];
}

- (void)helpButtonClicked
{
    if(self.tableViewType == CameraVideosTableView)
        [self showAcquireAuthorizationHelp];
    else if(self.tableViewType == UploadVideosTableView)
        [self showUploadVideoToPhoneHelp];
}
#pragma mark - update videoInfo list
- (void)doPreUpdate
{
    [videoInfoListUpdate removeAllObjects];
    [videoinfoListSwitch removeAllObjects];
    [self getAllVideoData];
    [self checkUpdate];
}
-(void)doPostUpdate
{
    NSMutableArray* tempTable = videoInfoList;
    videoInfoList = videoinfoListSwitch;
    videoinfoListSwitch = tempTable;
    [videoinfoListSwitch removeAllObjects];
    [videoInfoListUpdate removeAllObjects];
}
- (void)checkUpdate
{
    [self removeDeletedItems];
    [self addNewItems];
}
- (void)removeDeletedItems
{
    for(int i = 0; i < videoInfoList.count; i++)
    {
        BOOL find = NO;
        int index = -1;
        VideoInfo *oldItem = videoInfoList[i];
        for(int j = 0; j < videoInfoListUpdate.count; j++)
        {
            VideoInfo *newItem = videoInfoListUpdate[j];
            if([oldItem.fileURL isEqualToString:newItem.fileURL])
            {
                find = YES;
                index = j;
                break;
            }
        }
        if(find)
        {
            [videoinfoListSwitch addObject:oldItem];
            [videoInfoListUpdate removeObjectAtIndex:index];
        }
    }
}
-(void)addNewItems
{
    for(int i = 0; i < videoInfoListUpdate.count; i++)
    {
        VideoInfo *videoInfo = nil;
        VideoInfo *newItem = videoInfoListUpdate[i];
        if(self.tableViewType == CameraVideosTableView)
        {
            videoInfo = [self CreateCameraVideoInfoFromAVURLAsset:newItem.urlAsset phAsset:newItem.phAsset needInitThumbnail:YES];
        }
        else if(self.tableViewType == UploadVideosTableView)
        {
            videoInfo = [self createUploadVideoInfoFromFileURL:newItem.fileURL];
        }
        [videoinfoListSwitch insertObject:videoInfo atIndex:0];
    }
}

#pragma mark - init all videoInfo list
- (NSInteger)getVideosCount
{
    return [videoInfoList count];
}
-(void)checkBackgroundSetting
{
    if([videoInfoList count] <= 0)
    {
        [self showEmptyTableBackground];
    }
    else
    {
        [self hideEmptyTableBackground];
    }
}
- (void)initVideoData
{
    [self getAllVideoData];
    [self checkBackgroundSetting];
    [self.tableView reloadData];
}

- (void)getAllVideoData
{
    if(self.tableViewType == CameraVideosTableView)
    {
        [self initCameraVideos];
    }
    else if(self.tableViewType == UploadVideosTableView)
    {
        [self initUploadVideos];
    }
    else
    {
        [self initItunesVideos];
    }
}

- (void)initCameraVideos
{
//    MBProgressHUD *hubAlertView = [[MBProgressHUD alloc] initWithView:self.view];
//    hubAlertView.removeFromSuperViewOnHide = YES;
//    hubAlertView.minSize = CGSizeMake(60.f, 60.f);
//    [self.view addSubview:hubAlertView];
//    hubAlertView.labelText = NSLocalizedString(@"正在加载",nil);
//    [hubAlertView show:YES];

    NSString *finishedMsg = nil;
    if(videoFileFolder != nil)
    {
        [self loadAllMediaLibraryVideos];
        finishedMsg = NSLocalizedString(@"加载完毕", nil);
    }
    else
    {
        finishedMsg = NSLocalizedString(@"无相册访问权限", nil);
    }
//    MBProgressHUD *hud = [MBProgressHUD HUDForView:self.view];
//    hud.mode = MBProgressHUDModeAnnularDeterminate;
//    hud.labelText = finishedMsg;
//    [MBProgressHUD hideAllHUDsForView:self.view animated:YES];
}

- (void)initUploadVideos
{
    if(videoFileFolder != nil)
    {
        NSArray *uploadVideoFilesList = [[NSFileManager defaultManager] subpathsOfDirectoryAtPath:videoFileFolder error:nil];
        if(uploadVideoFilesList != nil)
        {
            videoInfoList = [NSMutableArray array];
            for(NSString *fileName in uploadVideoFilesList)
            {
                NSString *fileURL = [NSString stringWithFormat:@"%@/%@", videoFileFolder, fileName];
                if(doingUpdate)
                {
                    VideoInfo *videoInfo = [[VideoInfo alloc]init];
                    videoInfo.fileURL = fileURL;
                    [videoInfoListUpdate addObject:videoInfo];
                }
                else
                {
                    [videoInfoList insertObject:[self createUploadVideoInfoFromFileURL:fileURL] atIndex:0];
                }
            }
        }
    }
}

- (void)initItunesVideos
{
}

- (void)loadAllMediaLibraryVideos
{
    assetsFetchResults = [PHAssetCollection fetchAssetCollectionsWithType:PHAssetCollectionTypeSmartAlbum
                                                                  subtype:PHAssetCollectionSubtypeSmartAlbumVideos
                                                                  options:nil];
    for (PHAssetCollection *sub in assetsFetchResults)
    {
        PHFetchResult *assetsInCollection = [PHAsset fetchAssetsInAssetCollection:sub options:nil];
        for (PHAsset *phAsset in assetsInCollection)
        {
            NSArray *assetResources = [PHAssetResource assetResourcesForAsset:phAsset];
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
                    [[PHImageManager defaultManager] requestAVAssetForVideo:phAsset
                                                                    options:videoFetchOptions
                                                              resultHandler:^(AVAsset * _Nullable asset, AVAudioMix* _Nullable audioMix, NSDictionary* _Nullable info)
                     {
                         if([asset isKindOfClass:[AVURLAsset class]])
                         {
                             urlAsset = (AVURLAsset *)asset;
                         }
                         else
                         {
                             urlAsset = nil;
                         }
                         dispatch_semaphore_signal(semaphore);
                     }];
                    dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
                    if(urlAsset != nil)
                    {
                        if(doingUpdate)
                        {
                            VideoInfo *videoInfo = [[VideoInfo alloc]init];
                            videoInfo.phAsset = phAsset;
                            videoInfo.urlAsset = urlAsset;
                            videoInfo.fileURL = [urlAsset.URL absoluteString];
                            [videoInfoListUpdate addObject:videoInfo];
                        }
                        else
                        {
                            VideoInfo *videoInfo = [self CreateCameraVideoInfoFromAVURLAsset:urlAsset phAsset:phAsset needInitThumbnail:YES];
                            [videoInfoList insertObject:videoInfo atIndex:0];
                        }
                    }
                    [self updateTableViewUIAtMainThread];
                }
            }
        }
    }
}

#pragma mark - empty table background
- (void)hideEmptyTableBackground
{
    UIView *view = [self.tableView.backgroundView viewWithTag:1001];
    if(view != nil)
    {
        [view setHidden:YES];
    }
    if(_helpButton != nil)
    {
        [_helpButton setHidden:YES];
    }
}
- (void)showEmptyTableBackground
{
    [self setEmptyTableBackgroundImage];
    if(self.tableViewType == CameraVideosTableView)
    {
        if(self.accessMeidaRight == CanNotAccess)
        {
            [self addSetMediaAccessAuthoriztionHelpButton];
        }
    }
    else if(self.tableViewType == UploadVideosTableView)
    {
        [self addUploadVideoFileToPhoneHelpButton];
    }
    else {/* TODO */}
}

- (void)setEmptyTableBackgroundImage
{
    UIView *view = [self.tableView.backgroundView viewWithTag:1001];
    if(view == nil)
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
        bgView.tag = 1001;
        [self.tableView setBackgroundView:bgView];
    }
    else
    {
        [view setHidden:NO];
    }
}

- (void)addSetMediaAccessAuthoriztionHelpButton;
{
    if(_helpButton == nil)
    {
        NSString *buttonTitle = NSLocalizedString(@"获取相册权限",nil);
        [self createHelpButtonWithTitle:buttonTitle];
    }
    else
    {
        [_helpButton setHidden:NO];
    }
}

- (void)addUploadVideoFileToPhoneHelpButton;
{
    if(_helpButton == nil)
    {
        NSString *buttonTitle = NSLocalizedString(@"从电脑导入视频",nil);
        [self createHelpButtonWithTitle:buttonTitle];
    }
    else
    {
        [_helpButton setHidden:NO];
    }
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

- (VideoInfo*)CreateCameraVideoInfoFromAVURLAsset:(AVURLAsset*)urlAsset
                                          phAsset:(PHAsset*)phAsset
                                needInitThumbnail:(BOOL)needInitThumbnail
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
    videoInfo.phAsset = phAsset;
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
    return [self getVideosCount];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return 1;
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
        info = videoInfoList[indexPath.section];
    }
    else {/* TODO */}

    [cell setFrame:CGRectMake(0, 0, self.view.frame.size.width, 100)];
    [cell setVideoInfo:info];
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
        info = videoInfoList[indexPath.section];
    }
    else {/* TODO */}
    PlayVideoViewController *playView = [[PlayVideoViewController alloc]init];
    if(info != nil)
    {
        playView.parent = self;
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

- (void)playFailed
{
    NSString *notice = @"打开失败";
    if(self.tableViewType == CameraVideosTableView)
    {
        notice = @"文件已被删除";
    }
    else if(self.tableViewType == UploadVideosTableView)
    {
        notice = @"格式不支持";
    }
    [JRToast showWithText:NSLocalizedString(notice, nil) duration:1.2];
}

#pragma mark - tableView Edit
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath
{
    /* 相册视频，控制可以不删除
    if(self.tableViewType == UploadVideosTableView)
    {
        return YES;
    } */
    return YES;
}

-(UITableViewCellEditingStyle)tableView:(UITableView *)tableView editingStyleForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return UITableViewCellEditingStyleDelete;
}

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath
{
    [tableView setEditing:NO animated:YES];
    if (editingStyle == UITableViewCellEditingStyleDelete)
    {
        if(self.tableViewType == CameraVideosTableView)
        {
            [self removeMediaLibrayVideo:indexPath.section];
        }
        else if(self.tableViewType == UploadVideosTableView)
        {
            [self removeUploadVideo:indexPath.section];
        }
        else {/* TODO */}
    }
}

- (void)removeMediaLibrayVideo:(NSUInteger)index
{
    [self removeVideoCell:index];
}

- (void)removeUploadVideo:(NSUInteger)index
{
    UIAlertController *alert =[UIAlertController alertControllerWithTitle:@"确定删除该视频？"
                                                                  message:@""
                                                           preferredStyle:UIAlertControllerStyleAlert];
    UIAlertAction * cancle = [UIAlertAction actionWithTitle:@"取消" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action){
        NSLog(@"cancle");
    }];
    UIAlertAction * remove = [UIAlertAction actionWithTitle:@"删除" style:UIAlertActionStyleDestructive handler:^(UIAlertAction * _Nonnull action){
        NSLog(@"remove");
        [self.tableView beginUpdates];
        [self removeVideoCell:index];
        [self.tableView deleteSections:[NSIndexSet indexSetWithIndex:index] withRowAnimation:UITableViewRowAnimationLeft];
        [self.tableView endUpdates];
    }];
    [alert addAction:cancle];
    [alert addAction:remove];
    [self presentViewController:alert animated:YES completion:nil];
}

- (void)removeMediaLibraryVideoUpdate:(NSUInteger)index
{
    [videoInfoList removeObjectAtIndex:index];
    [self.tableView beginUpdates];
    [self.tableView deleteSections:[NSIndexSet indexSetWithIndex:index] withRowAnimation:UITableViewRowAnimationLeft];
    [self.tableView endUpdates];
    [self checkBackgroundSetting];
}

- (void)removeVideoCell:(NSUInteger)index
{
    if(index < [videoInfoList count])
    {
        VideoInfo *videoInfo = [videoInfoList objectAtIndex:index];
        if(videoInfo != nil)
        {
            if(self.tableViewType == CameraVideosTableView)
            {
                NSArray *asertArray = [[NSArray alloc] initWithObjects:videoInfo.phAsset, nil];
                [[PHPhotoLibrary sharedPhotoLibrary] performChanges:^{
                    [PHAssetChangeRequest deleteAssets:asertArray];
                }
                completionHandler:^(BOOL success, NSError *error){
                    if(success)
                    {
                        dispatch_sync(dispatch_get_main_queue(),^{
                            [self removeMediaLibraryVideoUpdate:index];
                        });
                    }
                }];
            }
            else if(self.tableViewType == UploadVideosTableView)
            {
                NSString *fileURL = videoInfo.fileURL;
                NSFileManager *fileManager = [NSFileManager defaultManager];
                if([fileManager fileExistsAtPath:fileURL])
                {
                    [fileManager removeItemAtPath:fileURL error:nil];
                    [videoInfoList removeObjectAtIndex:index];
                    [self checkBackgroundSetting];
                }
            }
            else {/* TODO */}
        }
    }
}
#pragma mark - rottation control
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
            //PHFetchResult *before = [collectionChanges fetchResultBeforeChanges];
            //PHFetchResult *after = [collectionChanges fetchResultAfterChanges];
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
