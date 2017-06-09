//
//  MainViewController.m
//  EPlayer
//
//  Created by Anthon Liu on 2017/6/9.
//  Copyright © 2017年 EC. All rights reserved.
//

#import "UINavigationBarEx.h"
#import "MainViewController.h"

#define NAVBAR_CHANGE_POINT 64
#define TABLE_VIEW_HEADER_H 200

@interface MainViewController ()
{
    UIImageView* tableViewHeaderImage;
}
@end

@implementation MainViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    [self.navigationController.navigationBar setHidden:YES];
    [self setTableViewHeaderView];
}

-(void)setTableViewHeaderView{
    UIView *aView = [[UIView alloc]initWithFrame:CGRectMake(0, 0, [UIScreen mainScreen].bounds.size.width, TABLE_VIEW_HEADER_H)];
    UIImageView* headerImageView = [[UIImageView alloc]initWithFrame:CGRectMake(0, 0, [UIScreen mainScreen].bounds.size.width, TABLE_VIEW_HEADER_H)];
    UIImage* image = [UIImage imageNamed:@"banner"];
    headerImageView.image = image;
    [aView addSubview:headerImageView];
    self.tableView.tableHeaderView = aView;
    self.tableView.contentInset = UIEdgeInsetsMake(-20, 0, 0, 0);
}

#pragma mark - Table view data source
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 0;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return 0;
}

- (void)scrollViewDidScroll:(UIScrollView *)scrollView
{
    scrollView.bounces = YES;
    if(scrollView.contentOffset.y > 0)
    {
        NSLog(@"up ,up ,up ,up.....");
    }
    else if(scrollView.contentOffset.y < 0)
    {
        NSLog(@"down ,down ,down ,down.....");
    }
    UIColor * color = [UIColor colorWithRed:0/255.0 green:175/255.0 blue:240/255.0 alpha:1];
    CGFloat offsetY = scrollView.contentOffset.y;
    if (offsetY > NAVBAR_CHANGE_POINT && offsetY < TABLE_VIEW_HEADER_H)
    {
        [self.navigationController.navigationBar setHidden:NO];
        CGFloat alpha = MIN(1, 1 - ((NAVBAR_CHANGE_POINT + 64 - offsetY) / 64));
        [self.navigationController.navigationBar lt_setBackgroundColor:[color colorWithAlphaComponent:alpha]];
    }
    else
    {
        [self.navigationController.navigationBar lt_setBackgroundColor:[color colorWithAlphaComponent:0]];
    }
    
    if(scrollView.contentOffset.y <= 0)
    {
        [self.tableView setContentOffset:CGPointMake(0, 0) animated:false];
    }
    else if(scrollView.contentOffset.y >= TABLE_VIEW_HEADER_H - 64)
    {
        scrollView.bounces = NO;
    }
}

- (BOOL)scrollViewShouldScrollToTop:(UIScrollView *)scrollView
{
    return NO;
}

/*
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:  forIndexPath:indexPath];
    
    // Configure the cell...
    
    return cell;
}
*/

/*
// Override to support conditional editing of the table view.
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return NO if you do not want the specified item to be editable.
    return YES;
}
*/

/*
// Override to support editing the table view.
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath {
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        // Delete the row from the data source
        [tableView deleteRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationFade];
    } else if (editingStyle == UITableViewCellEditingStyleInsert) {
        // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
    }   
}
*/

/*
// Override to support rearranging the table view.
- (void)tableView:(UITableView *)tableView moveRowAtIndexPath:(NSIndexPath *)fromIndexPath toIndexPath:(NSIndexPath *)toIndexPath {
}
*/

/*
// Override to support conditional rearranging of the table view.
- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return NO if you do not want the item to be re-orderable.
    return YES;
}
*/

/*
#pragma mark - Navigation
// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
