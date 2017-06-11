

#import <Photos/Photos.h>
#import "VideoInfoTableViewCell.h"
#import "VideoFilesTableViewController.h"

@interface VideoFilesTableViewController ()
{
    NSString *videoFileFolder;
    /* Upload Video files */
    NSArray *uploadVideoFileList;
    /* Camera Video files */
    NSMutableArray *cameraVideoFileList;
}
@end

static NSString *const CameraTablewCellIdentifier = @"CameraTablewCellIdentifier";
@implementation VideoFilesTableViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.tableView.dataSource = self;
    [self.tableView setBackgroundColor:[UIColor colorWithRed:236.0f/256.0f green:236.0f/256.0f blue:236.0f/256.0f alpha:1.0]];
    self.tableView.separatorStyle = UITableViewCellSeparatorStyleNone;
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerClass:[VideoInfoTableViewCell class] forCellReuseIdentifier:CameraTablewCellIdentifier];

    [self initAllVideoData];
    if([self getVideosCount] <= 0)
    {
        [self initEmptyTableBackground];
    }
}

- (void)setVideoFilesFolder :(NSString*)folderPath
{
    videoFileFolder = folderPath;
}

- (NSInteger)getVideosCount
{
    NSInteger count = 0;
    if(self.tableViewType == CameraVideosTableView)
    {
        count = [cameraVideoFileList count];
    }
    else if(self.tableViewType == UploadVideosTableView)
    {
        count = [uploadVideoFileList count];
    }
    else
    {
        /* TODO */
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
        uploadVideoFileList = [[NSFileManager defaultManager] subpathsOfDirectoryAtPath:videoFileFolder error:nil];
        if(uploadVideoFileList != nil)
        {
            filesCount = [uploadVideoFileList count];
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
    cameraVideoFileList = [[NSMutableArray alloc] init];

    PHFetchResult *assetsFetchResults = [PHAssetCollection fetchAssetCollectionsWithType:PHAssetCollectionTypeSmartAlbum
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
                    [[PHImageManager defaultManager] requestAVAssetForVideo:asset
                                                                    options:videoFetchOptions
                                                              resultHandler:^(AVAsset * _Nullable asset, AVAudioMix* _Nullable audioMix, NSDictionary* _Nullable info)
                     {
                         AVURLAsset *urlAsset = (AVURLAsset *)asset;
                         [cameraVideoFileList addObject:urlAsset];
                         [self.tableView reloadData];
                     }];
                }
            }
        }
    }
    return [cameraVideoFileList count];
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

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Config TableView cell
- (void)configVideoCell :(VideoInfoTableViewCell*)cell byFileURL:(NSString*)fileURL
{
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSDictionary *fileAttributes = [fileManager attributesOfItemAtPath:cell.fileURL error:nil];
    if (fileAttributes != nil)
    {
        NSDate *updateDate = [fileAttributes objectForKey:NSFileModificationDate];
        NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
        [dateFormatter setDateFormat:@"yyyy-MM-dd HH:mm:ss"];
        cell.updateTime = [dateFormatter stringFromDate:updateDate];
        cell.fileSize = [[fileAttributes objectForKey:NSFileSize] floatValue] / (1000*1000.0f);
    }
}

- (void)configCameraVideoCell :(VideoInfoTableViewCell*)cell fileIndex:(NSInteger)index
{
    cell.videoCellType = CameraVideoCell;
    AVURLAsset *urlAsset = [cameraVideoFileList objectAtIndex:index];
    if(urlAsset != nil)
    {
        cell.fileURL = [urlAsset.URL absoluteString];
        cell.fileName = [cell.fileURL lastPathComponent];
        cell.fileType = [cell.fileURL pathExtension];
        if(urlAsset.creationDate != nil)
        {
            NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
            [dateFormatter setDateFormat:@"yyyy-MM-dd HH:mm:ss"];
            cell.updateTime = [dateFormatter stringFromDate:urlAsset.creationDate.dateValue];
        }
        cell.fileSize = [[NSData dataWithContentsOfURL:urlAsset.URL] length] / (1000*1000.0f);
    }
    cell.urlAsset = urlAsset;
}

- (void)configUploadVideCell :(VideoInfoTableViewCell*)cell fileIndex:(NSInteger)index
{
    cell.videoCellType = UploadVideoCell;
    cell.fileName = uploadVideoFileList[index];
    cell.fileURL = [NSString stringWithFormat:@"%@/%@", videoFileFolder, cell.fileName];
    cell.fileType = [cell.fileName pathExtension];
    [self configVideoCell:cell byFileURL:cell.fileURL];
}

- (void)configItunesVideoCell :(VideoInfoTableViewCell*)cell
{
    cell.videoCellType = iTunesVideoCell;
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
    if(self.tableViewType == CameraVideosTableView)
    {
        [self configCameraVideoCell:cell fileIndex:indexPath.row];
    }
    else if(self.tableViewType == UploadVideosTableView)
    {
        [self configUploadVideCell:cell fileIndex:indexPath.row];
    }
    else
    {
        [self configItunesVideoCell:cell];
    }
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

@end
