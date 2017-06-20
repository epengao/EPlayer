
#import "WMPageController.h"
#import "UINavigationBarEx.h"

@interface MainViewController : WMPageController
@property (nonatomic, assign) CGFloat viewTop;
@property (nonatomic, assign) NSInteger playRet;
- (CGFloat) getTopViewHeight;
@end
