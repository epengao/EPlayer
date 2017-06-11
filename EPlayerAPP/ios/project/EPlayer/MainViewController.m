
#import "MainViewController.h"
#import <Photos/PHPhotoLibrary.h>
#import "WMPanGestureRecognizer.h"
#import <AVFoundation/AVMediaFormat.h>
#import "VideoFilesTableViewController.h"

typedef NS_ENUM(NSUInteger, AccessMediaLibraryRight)
{
    CanAccess     = 0,
    CanNotAccess  = 1,
    NeedRequest   = 2,
};

@interface MainViewController() <UIGestureRecognizerDelegate>
@property (nonatomic, strong) NSArray *videoCategories;
@property (nonatomic, strong) WMPanGestureRecognizer *panGesture;
@property (nonatomic, assign) CGPoint lastPoint;
@property (nonatomic, strong) UIView *redView;
@property (nonatomic, assign) AccessMediaLibraryRight accessMeidaRight;
@end

@implementation MainViewController

- (NSArray *)videoCategories
{
    if (!_videoCategories)
    {
        _videoCategories = @[@"相册视频", @"EPlayer媒体", @"iTunes"];
    }
    return _videoCategories;
}

- (instancetype)init
{
    if (self = [super init])
    {
        self.titleSizeNormal = 15;
        self.titleSizeSelected = 15;
        self.menuViewStyle = WMMenuViewStyleFloodHollow;
        self.menuItemWidth = [UIScreen mainScreen].bounds.size.width / self.videoCategories.count;
        self.menuHeight = 40;
        self.viewTop = kWMHeaderViewHeight;
        self.accessMeidaRight = NeedRequest;
        self.titleColorSelected = [UIColor colorWithRed:90.0f/256.0f green:53.0f/256.0f blue:200.0f/256.0f alpha:1.0];
        self.titleColorNormal = [UIColor colorWithRed:135.0f/256.0f green:120.0f/256.0f blue:185.0f/256.0f alpha:1.0];
        self.menuBGColor = [UIColor whiteColor];
        self.titleFontName = @"Helvetica-Bold";
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    self.title = @"EPlayer 易播";
    UIView *redView = [[UIView alloc] initWithFrame:CGRectMake(0, kNavigationBarHeight, [UIScreen mainScreen].bounds.size.width, kWMHeaderViewHeight)];
    redView.backgroundColor = [UIColor redColor];
    
    UIView *aView = [[UIView alloc]initWithFrame:CGRectMake(0, 0, [UIScreen mainScreen].bounds.size.width, kWMHeaderViewHeight)];
    UIImageView* headerImageView = [[UIImageView alloc]initWithFrame:CGRectMake(0, 0, [UIScreen mainScreen].bounds.size.width, kWMHeaderViewHeight)];
    UIImage* image = [UIImage imageNamed:@"banner"];
    headerImageView.image = image;
    [aView addSubview:headerImageView];
    
    UIColor * color = [UIColor colorWithRed:90.0f/256.0f green:53.0f/256.0f blue:200.0f/256.0f alpha:0.0];
    [self.navigationController.navigationBar lt_setBackgroundColor:[color colorWithAlphaComponent:0]];
    
    self.redView = aView;
    [self.view addSubview:self.redView];
    self.panGesture = [[WMPanGestureRecognizer alloc] initWithTarget:self action:@selector(panOnView:)];
    [self.view addGestureRecognizer:self.panGesture];
    
    NSDictionary *attributes=[NSDictionary dictionaryWithObjectsAndKeys:
                              [UIColor whiteColor],
                              NSForegroundColorAttributeName,
                              [UIFont systemFontOfSize:18],
                              NSFontAttributeName,
                              nil];
    [self.navigationController.navigationBar setTitleTextAttributes:attributes];
    /* 电池栏的字体颜色设置 */
    self.navigationController.navigationBar.barStyle = UIStatusBarStyleLightContent;
    [self setNeedsStatusBarAppearanceUpdate];
}

- (void)btnClicked:(id)sender
{
    NSLog(@"touch up inside");
}

- (void)updateNavigationAlpha:(CGFloat)alpha
{
    UIColor * color = [UIColor colorWithRed:90.0f/256.0f green:53.0f/256.0f blue:200.0f/256.0f alpha:1.0];
    [self.navigationController.navigationBar lt_setBackgroundColor:[color colorWithAlphaComponent:alpha]];
    NSDictionary *attributes=[NSDictionary dictionaryWithObjectsAndKeys:
                              [UIColor colorWithWhite:1.0 alpha:alpha],
                              NSForegroundColorAttributeName,
                              [UIFont systemFontOfSize:18],
                              NSFontAttributeName,
                              nil];
    [self.navigationController.navigationBar setTitleTextAttributes:attributes];
}

- (void)panOnView:(WMPanGestureRecognizer *)recognizer
{
    CGPoint currentPoint = [recognizer locationInView:self.view];
    
    if (recognizer.state == UIGestureRecognizerStateBegan) {
        self.lastPoint = currentPoint;
    } else if (recognizer.state == UIGestureRecognizerStateEnded) {
        
        CGPoint velocity = [recognizer velocityInView:self.view];
        CGFloat targetPoint = velocity.y < 0 ? kNavigationBarHeight : kNavigationBarHeight + kWMHeaderViewHeight;
        NSTimeInterval duration = fabs((targetPoint - self.viewTop) / velocity.y);
        
        if (fabs(velocity.y) * 1.0 > fabs(targetPoint - self.viewTop))
        {
            [UIView animateWithDuration:duration delay:0 options:UIViewAnimationOptionCurveEaseOut animations:^{
                self.viewTop = targetPoint;
            } completion:nil];
            
            return;
        }
    }
    CGFloat yChange = currentPoint.y - self.lastPoint.y;
    
    self.viewTop += yChange;
    self.lastPoint = currentPoint;
}

- (void)setViewTop:(CGFloat)viewTop
{
    _viewTop = viewTop;
    
    CGFloat factor = 1 - (CGFloat)(_viewTop-kNavigationBarHeight)/(CGFloat)(kWMHeaderViewHeight-kNavigationBarHeight);
    if(factor <= 0) factor = 0.0f;
    if(factor >= 1) factor = 1.0f;
    [self updateNavigationAlpha:factor];
    
    if (_viewTop <= kNavigationBarHeight)
    {
        _viewTop = kNavigationBarHeight;
    }
    if (_viewTop > kWMHeaderViewHeight)
    {
        _viewTop = kWMHeaderViewHeight;
    }
    
    self.redView.frame = (
    {
        CGRect oriFrame = self.redView.frame;
        oriFrame.origin.y = _viewTop - kWMHeaderViewHeight;
        oriFrame;
    });
    self.viewFrame = CGRectMake(0, _viewTop, [UIScreen mainScreen].bounds.size.width, [UIScreen mainScreen].bounds.size.height - _viewTop);

}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

#pragma mark - Datasource & Delegate
- (NSInteger)numbersOfChildControllersInPageController:(WMPageController *)pageController
{
    return self.videoCategories.count;
}

- (UIViewController *)pageController:(WMPageController *)pageController viewControllerAtIndex:(NSInteger)index
{
    VideoFilesTableViewController *vc = [[VideoFilesTableViewController alloc] init];
    if(index == 0)
    {
        NSString *folder = nil;
        AccessMediaLibraryRight right = [self checkMediaLibraryAccessRight];
        if(right == NeedRequest)
        {
            right = [self acquireMediaLibraryAccessRight];
        }
        if(right == CanAccess)
        {
            folder = @"CameraLibraryFolder";
        }
        [vc setVideoFilesFolder:folder];
        vc.tableViewType = CameraVideosTableView;
    }
    else if (index == 1)
    {
        NSArray *patchs = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *documentsDirectory = [patchs objectAtIndex:0];
        NSString *fileDirectory = [documentsDirectory stringByAppendingPathComponent:@""];
        [vc setVideoFilesFolder:fileDirectory];
        vc.tableViewType = UploadVideosTableView;
    }
    else
    {
        [vc setVideoFilesFolder:nil];
        vc.tableViewType = iTunesVideosTableView;
    }
    return vc;
}

- (NSString *)pageController:(WMPageController *)pageController titleAtIndex:(NSInteger)index
{
    return self.videoCategories[index];
}

- (AccessMediaLibraryRight)acquireMediaLibraryAccessRight
{
    self.accessMeidaRight = NeedRequest;
    [PHPhotoLibrary requestAuthorization:^(PHAuthorizationStatus status){
        dispatch_async(dispatch_get_main_queue(), ^{
            switch (status) {
                case PHAuthorizationStatusAuthorized:
                {
                    self.accessMeidaRight = CanAccess;
                    break;
                }
                default:
                {
                    self.accessMeidaRight = CanNotAccess;
                    break;
                }
            }
        });
    }];
    return self.accessMeidaRight;
}

- (AccessMediaLibraryRight)checkMediaLibraryAccessRight
{
    AccessMediaLibraryRight ret = NeedRequest;
    PHAuthorizationStatus status = [PHPhotoLibrary authorizationStatus];
    switch (status)
    {
        case PHAuthorizationStatusAuthorized:
        {
            ret = CanAccess;
            break;
        }
        case PHAuthorizationStatusRestricted:
        case PHAuthorizationStatusDenied:
        {
            ret = CanNotAccess;
            break;
        }
        case PHAuthorizationStatusNotDetermined:
        {
            ret = NeedRequest;
            break;
        }
    }
    return ret;
}
@end
