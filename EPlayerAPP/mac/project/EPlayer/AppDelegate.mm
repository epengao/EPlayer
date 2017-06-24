//
//  AppDelegate.m
//  EPlayer
//
//  Created by GaoPeng on 2017/5/13.
//  Copyright © 2017年 EC. All rights reserved.
//

#import "EPlayerAPI.h"
#import "AppDelegate.h"
#import "WinTitleView.h"
#import "MediaCtrlView.h"
#import "ISSoundAdditions.h"
#import "HelpWindowController.h"

@interface AppDelegate ()
/* EPlayer SDK */
@property MediaInfo* mediaInfo;
@property EPlayerAPI* playerSDK;
/* VideoWindow */
@property VideoWindow *videoWnd;
/* player init flag */
@property bool playerSdkInitFinished;
/* NSWindow Title */
@property WinTitleView * titleview;
/* Current NSWindow */
@property (weak) IBOutlet MainWindow *window;
/* File menus Ctrl */
@property (weak) IBOutlet NSMenuItem *openURLMenu;
@property (weak) IBOutlet NSMenuItem *closeMediaMenu;
/* Audio Ctrl Menu Items */
@property (weak) IBOutlet NSMenuItem *audioMute;
@property (weak) IBOutlet NSMenuItem *audioDecreaseVolume;
@property (weak) IBOutlet NSMenuItem *audioIncreaseVolume;
/* Video Ctrl Menu Items */
@property (weak) IBOutlet NSMenuItem *videoResizeHalf;
@property (weak) IBOutlet NSMenuItem *videoResizeNormal;
@property (weak) IBOutlet NSMenuItem *videoResizeDouble;
@property (weak) IBOutlet NSMenuItem *videoResizeEntryFullScreen;
/* Window Ctrl */
@property (weak) IBOutlet NSMenuItem *windowInFront;
/* Media Ctrl Panel */
@property (weak) IBOutlet MediaCtrlView *MediaCtrPanel;
/* Help window controller */
@property HelpWindowController* helpWindowController;
/* Video Window background Icon */
@property (weak) IBOutlet NSImageView *backgroundImageView;
/* Mouse moved event trace */
@property NSTrackingArea* traceArea;
@end

/* NSView extension [静态毛玻璃效果] */
@interface NSView (Vibrancy)
- (instancetype)insertVibrancyViewBlendingMode:(NSVisualEffectBlendingMode)mode;
@end
@implementation NSView (Vibrancy)
- (instancetype)insertVibrancyViewBlendingMode:(NSVisualEffectBlendingMode)mode
{
    Class vibrantClass=NSClassFromString(@"NSVisualEffectView");
    if (vibrantClass)
    {
        NSVisualEffectView *vibrant=[[vibrantClass alloc] initWithFrame:self.bounds];
        [vibrant setAutoresizingMask:NSViewWidthSizable|NSViewHeightSizable];
        [vibrant setBlendingMode:mode];
        [vibrant setMaterial:NSVisualEffectMaterialDark];
        [self addSubview:vibrant positioned:NSWindowBelow relativeTo:nil];
        
        return vibrant;
    }
    return nil;
}
@end

@implementation AppDelegate

- (id)init
{
    self = [super init];
    if (self)
    {
        self.playerSdkInitFinished = false;
    }
    return self;
}

- (void)initMediaCtrlPanel
{
    [_MediaCtrPanel initShadowView];
    [_MediaCtrPanel initVolumeCtrl];
    [_MediaCtrPanel initProgressBar];
    [_MediaCtrPanel initPlayStopButton];
    [_MediaCtrPanel initPlayPauseButton];
    [_MediaCtrPanel initFullScreenButton];

    [self updateMediaCtrlPanelSize];
    [_window.contentView addSubview:_MediaCtrPanel];
}

- (void)updateMediaCtrlPanelSize
{
    CGFloat x = 0;
    CGFloat y = 0;
    CGFloat w = _window.frame.size.width;
    CGFloat h = 76;
    [_MediaCtrPanel updateViewFrame:NSMakeRect(x, y, w, h)];
}

#pragma mark - Application main window init
-(void) initMeidaWindow
{
    self.window.appCtrl = self;
    self.window.delegate = self;
    self.helpWindowController = nil;
    [self.window setMovableByWindowBackground:YES];
    self.window.contentMinSize = NSMakeSize(260, 220);
    self.window.titleVisibility = NSWindowTitleHidden;
    self.videoWnd = [VideoWindow createVideoWindowFrom:self.window];
    //self.window.collectionBehavior = NSWindowCollectionBehaviorFullScreenNone;

    CGFloat screenWidth = [NSScreen mainScreen].frame.size.width;
    CGFloat screenHeight = [NSScreen mainScreen].frame.size.height;
    CGFloat initWndWidth = screenWidth * 0.4;
    CGFloat initWndHeight = initWndWidth * 0.618 + 30;
    CGFloat x = (screenWidth - initWndWidth) * 0.5;
    CGFloat y = (screenHeight - initWndHeight) * 0.5;
    [self.window setFrame:NSMakeRect(x, y, initWndWidth, initWndHeight) display:YES];

    NSRect boundsRect = [[[self.window contentView] superview] bounds];
    _titleview = [[WinTitleView alloc] initWithFrame:boundsRect];
    [_titleview setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];

    self.window.backgroundColor = [NSColor colorWithDeviceRed:0 green:0 blue:0 alpha:1.00];
    [self.window setOpaque:NO];

    [[[self.window contentView] superview] addSubview:_titleview positioned:NSWindowBelow relativeTo:[[[[self.window contentView] superview] subviews] objectAtIndex:0]];

    NSButton *closeButton = [[self window] standardWindowButton:NSWindowCloseButton];
    [closeButton setTarget:self];
    [closeButton setAction:@selector(closeApplication)];

    /* 静态毛玻璃模糊 */
    //[self.window.contentView insertVibrancyViewBlendingMode:NSVisualEffectBlendingModeWithinWindow];
    /* We will supoort OpenURL later */
    [self.openURLMenu setHidden:YES];

    [self setAudioMenuItems:NO];
    [self setVideoMenuItems:NO];
    [self setMenuStatus:_closeMediaMenu status:NO action:nil];
}

-(void)initVideoWindowBackgroundImage
{
    [self.backgroundImageView setImage:[NSImage imageNamed:@"AppIcon"]];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    [self initMeidaWindow];
    [self initMediaCtrlPanel];
    [self initVideoWindowBackgroundImage];
    
    self.playerSDK = [EPlayerAPI sharedEPlayerAPI];
    self.playerSDK.msgHandler = self;
    self.playerSdkInitFinished = true;

    _MediaCtrPanel.mainCtrl = self;
    _MediaCtrPanel.playerSDK = _playerSDK;
    [_MediaCtrPanel updatePlayPauseUI:NO];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification
{
}

- (void) closeApplication {
    if([_playerSDK hasMediaActived])
    {
        [_MediaCtrPanel updateProgressBarInfo:0.0 max:0.0 currValue:0.0 runing:NO];
        [_playerSDK closeMedia];
    }
    [[NSApplication sharedApplication] terminate:nil];
}

#pragma mark - CloseMedia and Update UI
- (void) closeMediaWithUpdateUI
{
    [_playerSDK closeMedia];
    [_MediaCtrPanel updatePlayPauseUI:NO];
    [_MediaCtrPanel updateProgressBarInfo:0.0 max:100.0 currValue:0.0 runing:NO];
    [self setAudioMenuItems:NO];
    [self setVideoMenuItems:NO];
    [self setMenuStatus:_closeMediaMenu status:NO action:nil];
    [self.window.contentView setNeedsDisplay:YES];
    [self.window setViewsNeedDisplay:YES];
    self.window.ctrViewNeedDisplay = YES;
    [self updateMediaCtrlPanelVisible];
    [self.backgroundImageView setHidden:NO];
    if(self.window.isFullScreen)
    {
        [self performSelector:@selector(entryFullScreen) withObject:nil afterDelay:0.1];
//        dispatch_sync(dispatch_get_main_queue(),^
//        {
//            [self entryFullScreen];
//        });
    }
    //self.window.collectionBehavior = NSWindowCollectionBehaviorFullScreenNone;
}

//- (NSSize)windowWillResize:(NSWindow *)window toSize:(NSSize)proposedFrameSize
//{
//    NSUInteger targetW = proposedFrameSize.width;
//    NSUInteger targetH = proposedFrameSize.height;
//    
//    if(targetW < 260)
//    {
//        targetW = 260;
//        targetH = proposedFrameSize.height/proposedFrameSize.width * targetW;
//    }
//    if(targetH < 220)
//    {
//        targetH = 220;
//        targetW = proposedFrameSize.width/proposedFrameSize.height * targetH;
//    }
//    proposedFrameSize.width = targetW;
//    proposedFrameSize.height = targetH;
//    return proposedFrameSize;
//}

#pragma mark - Window Resize Procedure
- (void)windowDidResize:(NSNotification *)aNotification
{
    int ctrlViewHeight = 76;
    CGFloat screenWidth = [NSScreen mainScreen].frame.size.width;
    CGFloat screenHeight = [NSScreen mainScreen].frame.size.height;
    CGFloat wndWidth = self.window.contentView.frame.size.width;
    CGFloat wndHeight = self.window.contentView.frame.size.height;
    if(screenWidth == wndWidth && screenHeight == wndHeight)
    {
        ctrlViewHeight = 0;
    }
    CGFloat factor = [NSScreen mainScreen].backingScaleFactor;
    int width = wndWidth * factor;
    int height = (wndHeight - ctrlViewHeight) * factor;
    if([_playerSDK hasMediaActived] && _mediaInfo.hasVideo)
    {
        [_playerSDK updateVideoWindow:self.videoWnd windowWidth:width windowHeight:height];
    }
    [self updateMediaCtrlPanelSize];
}

-(void)resizeVideoWindowByRatio:(double)ratio
{
    CGFloat videoWidth = _mediaInfo.videoWidth;
    CGFloat videoHeight = _mediaInfo.videoHeight;
    CGFloat factor = [NSScreen mainScreen].backingScaleFactor;

    CGFloat width = (videoWidth / factor) * ratio;
    CGFloat height = (videoHeight / factor) * ratio + 76;
    if(width < 260 || height < 220)
    {
        width = 260;
        height = 220;
    }
    if(self.window.contentView.frame.size.width != width ||
       self.window.contentView.frame.size.height!= height )
    {
        CGFloat titleHeight = [_titleview getTitleHeight];
        CGFloat screenWidth = [NSScreen mainScreen].frame.size.width;
        CGFloat screenHeight = [NSScreen mainScreen].frame.size.height;
        CGFloat x = (screenWidth - width) * 0.5;
        CGFloat y = (screenHeight - height) * 0.5;
        [self.window setFrame:NSMakeRect(x, y, width, height + titleHeight) display:YES];
    }
}

//-(void)windowDidChangeScreen:(NSNotification *)notification
//{
//    [self.window setFrame:self.window.frame display:YES animate:YES];
//}

-(void)setMenuStatus:(NSMenuItem*)menu status:(BOOL)enable action:(SEL)method
{
    if(enable)
    {
        [menu setTarget:self];
        [menu setAction:method];
    }
    else
    {
        [menu setTarget:nil];
        [menu setAction:nil];
    }
}

-(void)setAudioMenuItems:(BOOL)enable
{
    /*
     * Just keep the menu status sync with vluem icon
     * not set the status
    if(!enable)
    {
        [NSSound applyMute:NO];
        [_audioMute setState:NSOffState];
    }
    */

    [self setMenuStatus:_audioMute status:enable action:@selector(menuAudioMute:)];
    [self setMenuStatus:_audioIncreaseVolume status:enable action:@selector(menuAudioIncreaseVolume:)];
    [self setMenuStatus:_audioDecreaseVolume status:enable action:@selector(menuAudioDecreaseVolume:)];
}

-(void)setVideoMenuItems:(BOOL)enable
{
    [self setMenuStatus:_videoResizeHalf status:enable action:@selector(menuResizeHalf:)];
    [self setMenuStatus:_videoResizeNormal status:enable action:@selector(menuResizeNormal:)];
    [self setMenuStatus:_videoResizeDouble status:enable action:@selector(menuResizeDouble:)];
    [self setMenuStatus:_videoResizeEntryFullScreen status:enable action:@selector(menuEentryFullScreen:)];
}

#pragma mark - EPlayerSDK delegate
-(void)handleEPlayerMsg:(EPlayerMessage)msg
{
    if(msg == EPlayer_Msg_PlayStop)
    {
        dispatch_sync(dispatch_get_main_queue(),^
        {
            [self closeMediaWithUpdateUI];
        });
    }
}

#pragma mark - menu openDocument
-(IBAction)menuOpenDocument:(id)sender
{
    NSOpenPanel *openDocPanel = [NSOpenPanel openPanel];

    [openDocPanel setCanChooseFiles:YES];
    [openDocPanel setCanChooseDirectories:NO];
    [openDocPanel setAllowsMultipleSelection:NO];

    if ([openDocPanel runModal] == NSModalResponseOK)
    {
        NSString* mediaPath = [[openDocPanel.URL absoluteString]stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
        [self openMedia:mediaPath];
    }
}

-(IBAction)menuOpenURL:(id)sender
{
    
}

-(IBAction)menuCloseMedia:(id)sender
{
    if([_playerSDK hasMediaActived])
    {
        [self closeMediaWithUpdateUI];
    }
}

-(IBAction)menuAudioMute:(id)sender
{
    if(NSOnState == _audioMute.state)
    {
        [NSSound applyMute:NO];
        [_audioMute setState:NSOffState];
    }
    else
    {
        [NSSound applyMute:YES];
        [_audioMute setState:NSOnState];
    }
    [_MediaCtrPanel updateVolumeUI:_audioMute.state];
}

-(IBAction)menuAudioIncreaseVolume:(id)sender
{
    [NSSound increaseSystemVolumeBy:0.1];
}

-(IBAction)menuAudioDecreaseVolume:(id)sender
{
    [NSSound decreaseSystemVolumeBy:0.1];
}

-(IBAction)menuResizeHalf:(id)sender
{
    [self resizeVideoWindowByRatio:0.5];
}

-(IBAction)menuResizeNormal:(id)sender
{
    [self resizeVideoWindowByRatio:1];
}

-(IBAction)menuResizeDouble:(id)sender
{
    [self resizeVideoWindowByRatio:2];
}

-(IBAction)menuEentryFullScreen:(id)sender
{
    [self entryFullScreen];
}

-(IBAction)menuFitToScreen:(id)sender
{
    [self.window setFrame:[[NSScreen mainScreen] visibleFrame] display:YES animate:YES];
}

-(IBAction)menuAlwaysInFront:(id)sender
{
    if(NSOnState == _windowInFront.state)
    {
        [self.window setLevel:NSNormalWindowLevel];
        [_windowInFront setState:NSOffState];
    }
    else
    {
        [self.window setLevel:NSFloatingWindowLevel];
        [_windowInFront setState:NSOnState];
    }
}

-(IBAction)menuOpenHelpWindow:(id)sender
{
    if(self.helpWindowController == nil)
    {
        self.helpWindowController = [[HelpWindowController alloc] initWithWindowNibName:@"HelpWindowController"];
        [self.helpWindowController showWindow:self.window];
    }
    else
    {
        [self.helpWindowController.window makeKeyAndOrderFront:nil];
    }
}

-(IBAction)menuEPlayerHomePage:(id)sender
{
    [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString: @"http://www.eplayer.me"]];
}

-(IBAction)menuEPlayerSourceCode:(id)sender
{
    [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString: @"http://www.eplayer.me"]];
}

#pragma mark - Exit/Entry FullScreen
- (void)windowDidEnterFullScreen:(NSNotification *)notification
{
    self.window.isFullScreen = YES;
    self.window.ctrViewNeedDisplay = NO;
    [self performSelector:@selector(updateMediaCtrlPanelVisible) withObject:nil afterDelay:0.5];

    CGRect rect = CGRectMake(0.0f, 0.0f, self.window.frame.size.width, self.window.frame.size.height);
    NSTrackingAreaOptions options = NSTrackingActiveInKeyWindow | NSTrackingMouseMoved | NSTrackingInVisibleRect;
    _traceArea = [[NSTrackingArea alloc] initWithRect:rect options:options owner:self.window userInfo:nil];
    [self.window.contentView addTrackingArea:_traceArea];
}

- (void)windowDidExitFullScreen:(NSNotification *)notification
{
    [self.window.contentView removeTrackingArea:_traceArea];
    self.window.isFullScreen = NO;
    self.window.ctrViewNeedDisplay = YES;
    [self.window stopHideCtrlPannelTimer];
    [self updateMediaCtrlPanelVisible];
    [self.MediaCtrPanel setHidden:NO];
}

- (void)entryFullScreen
{
    if(NSOnState == _videoResizeEntryFullScreen.state)
    {
        [_videoResizeEntryFullScreen setState:NSOffState];
    }
    else
    {
        [_videoResizeEntryFullScreen setState:NSOnState];
    }
    [self.window setFrame:[NSScreen mainScreen].visibleFrame display:YES];
    [self.window toggleFullScreen:self];
}

- (void)updateMediaCtrlPanelVisible
{
    if(self.window.isFullScreen && !(self.window.ctrViewNeedDisplay))
    {
        [_MediaCtrPanel setHidden:YES];
    }
    else
    {
        [_MediaCtrPanel setHidden:NO];
    }
}

#pragma mark - medaiCtrl Panel notify
- (void)volumeMuteNotify:(BOOL)mute
{
    [_audioMute setState:mute];
}

- (void)mediaStopedNotify
{
    [self closeMediaWithUpdateUI];
    //[self performSelector:@selector(closeMediaWithUpdateUI) withObject:nil afterDelay:0.1];
}

-(void)openMedia:(NSString*)mediaURL
{
    if([_playerSDK hasMediaActived])
    {
        [_playerSDK closeMedia];
    }

    int ctrlViewHeight = 76;
    if(self.window.isFullScreen)
    {
        ctrlViewHeight = 0;
    }
    CGFloat factor = [NSScreen mainScreen].backingScaleFactor;
    unsigned int width = self.window.contentView.frame.size.width * factor;
    unsigned int height = (self.window.contentView.frame.size.height - ctrlViewHeight) * factor;
    NSInteger ret = [_playerSDK openMediaPath:mediaURL videoWindow:self.videoWnd windowWidth:width windowHeight:height];
    if(ret == EPlayer_Err_None)
    {
        _mediaInfo = [_playerSDK getMeidaInfo];
        _MediaCtrPanel.mediaInfo = _mediaInfo;
        if(_mediaInfo.hasAudio)
        {
            [self setAudioMenuItems:YES];
        }
        if(_mediaInfo.hasVideo)
        {
            [self setVideoMenuItems:YES];
        }
        [self.backgroundImageView setHidden:YES];
        [self setMenuStatus:_closeMediaMenu status:YES action:@selector(menuCloseMedia:)];
        float duration = _mediaInfo.duration;
        [_MediaCtrPanel updatePlayPauseUI:YES];
        if(_mediaInfo.seekable)
        {
            [_MediaCtrPanel updateProgressBarInfo:0.0 max:duration currValue:0.0 runing:YES];
        }
        //self.window.collectionBehavior = NSWindowCollectionBehaviorFullScreenPrimary;
        [_playerSDK play];
        if(!self.window.isFullScreen && _mediaInfo.hasVideo)
        {
            [self resizeVideoWindowByRatio:1];
        }
    }
}

@end

@implementation MainWindow
#pragma mark - keyboard event

#define R_Key     15
#define EscKey    53
#define EnterKey  36
#define ReturnKey 76
#define SpaceKey  49

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (void)keyDown:(NSEvent *)event
{
    NSUInteger keyCode = [event keyCode];
    if(keyCode == EnterKey || keyCode == ReturnKey)
    {
        [self.appCtrl entryFullScreen];
    }
    else if(keyCode == SpaceKey)
    {
        if([self.appCtrl.playerSDK hasMediaActived])
        {
            EPlayerStatus status = [self.appCtrl.playerSDK getPlayerStatus];
            if(status == EPlayerStatus_Playing)
            {
                [self.appCtrl.playerSDK pause];
                [self.appCtrl.MediaCtrPanel updatePlayPauseUI:NO];
            }
            else if(status == EPlayerStatus_Paused)
            {
                [self.appCtrl.playerSDK play];
                [self.appCtrl.MediaCtrPanel updatePlayPauseUI:YES];
            }
        }
    }
    else if(keyCode == EscKey)
    {
        if(self.isFullScreen)
        {
            [self.appCtrl entryFullScreen];
        }
    }
    else if ([event modifierFlags] & NSCommandKeyMask)
    {
        if(keyCode == R_Key)
        {
            NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
            if ([[pasteboard types] containsObject:NSPasteboardTypeString])
            {
                NSString *mediaURL = [pasteboard stringForType:NSPasteboardTypeString];
                [self.appCtrl openMedia:mediaURL];
            }
        }
    }
}

#pragma mark - mounse event
- (BOOL)acceptsMouseMovedEvents
{
    return YES;
}

- (void)mouseDown:(NSEvent *)event
{
    if(self.isFullScreen)
    {
        /* Current show */
        self.ctrViewNeedDisplay = YES;
        [self stopHideCtrlPannelTimer];
        [self.appCtrl.MediaCtrPanel setHidden:NO];
        /* 3.0 second after will hide */
        self.ctrViewNeedDisplay = NO;
        [self hideMediaCtrlPannelAfterTime:3.0];
    }
}

- (void)mouseMoved:(NSEvent *)event
{
    if(self.isFullScreen)
    {
        /* Current show */
        self.ctrViewNeedDisplay = YES;
        [self stopHideCtrlPannelTimer];
        [self.appCtrl.MediaCtrPanel setHidden:NO];
        /* 1.5 second after will hide */
        self.ctrViewNeedDisplay = NO;
        [self hideMediaCtrlPannelAfterTime:1.5];
    }
}

- (void)mouseDragged:(NSEvent *)event;
{
}

-(void)rightMouseDown:(NSEvent *)event
{
}

-(void)mouseUp:(NSEvent *)event
{
}

-(void)mouseEntered:(NSEvent *)event
{
    NSLog(@"mouseEntered");
}

-(void)mouseExited:(NSEvent *)event
{
    NSLog(@"mouseExited");
}

#pragma mark - CtrlView Manager
- (void)updateCtrlViewVisiable
{
    [self.appCtrl updateMediaCtrlPanelVisible];
    self.ctrViewNeedDisplay = NO;
}
- (void)hideMediaCtrlPannelAfterTime:(float)time
{
    _ctrViewWatcher = [NSTimer scheduledTimerWithTimeInterval:time
                                                       target:self
                                                     selector:@selector(updateCtrlViewVisiable)
                                                     userInfo:nil
                                                      repeats:NO];
}
- (void)stopHideCtrlPannelTimer
{
    if(_ctrViewWatcher)
    {
        [_ctrViewWatcher invalidate];
        _ctrViewWatcher = nil;
    }
}
@end
