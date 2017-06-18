
#import "WMPageController.h"
#import "UINavigationBarEx.h"

static CGFloat const kWMHeaderViewHeight = 200;
static CGFloat const kNavigationBarHeight = 64;
@interface MainViewController : WMPageController
@property (nonatomic, assign) CGFloat viewTop;
@property (nonatomic, assign) NSInteger playRet;
- (CGFloat) getTopViewHeight;
@end
