
#import "VideoInfoTableViewCell.h"
#import "CameraVideoTableViewController.h"

@interface CameraVideoTableViewController ()

@end

static NSString *const CameraTablewCellIdentifier = @"CameraTablewCellIdentifier";
@implementation CameraVideoTableViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    [self.tableView setBackgroundColor:[UIColor colorWithRed:236.0f/256.0f green:236.0f/256.0f blue:236.0f/256.0f alpha:1.0]];
    self.tableView.separatorStyle = UITableViewCellSeparatorStyleNone;
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerClass:[VideoInfoTableViewCell class] forCellReuseIdentifier:CameraTablewCellIdentifier];
    
    [self InitEmptyTableBackground];
}

- (void)InitEmptyTableBackground
{
    UIImage* image = [UIImage imageNamed:@"no_video_bg"];
    CGSize tableSize = self.tableView.frame.size;
    CGFloat targetWidth = tableSize.width * 0.5;
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

#pragma mark - Table view data source
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 0;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return 0;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    VideoInfoTableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CameraTablewCellIdentifier];
    if (cell == nil)
    {
        cell = [[VideoInfoTableViewCell alloc] initWithStyle:UITableViewCellStyleDefault
                                             reuseIdentifier:CameraTablewCellIdentifier];
    }
    cell.textLabel.text = @"歌曲  000";
    [cell setFrame:CGRectMake(0, 0, self.view.frame.size.width - 10, 100)];
    return cell;
}

- (CGFloat)tableView:(UITableView*)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return 100;
//    VideoInfoTableViewCell *cell =(VideoInfoTableViewCell *)([self.tableView dequeueReusableCellWithIdentifier:CameraTablewCellIdentifier forIndexPath:indexPath]);
//    return cell.frame.size.height;
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
@end
