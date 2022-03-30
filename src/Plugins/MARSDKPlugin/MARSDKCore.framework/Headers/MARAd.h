//
//  MARAd.h
//  MARSDKCore
//
//  Created by js wu on 2021/6/24.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

//  广告错误类型
typedef NS_ENUM(NSInteger, MARAdErrorCode)
{
    SA_ERROR_AD_LOAD_FAILED = 1,
    SA_ERROR_AD_ERROR
};

//  广告图片对象
@interface MARAdImageInfo : NSObject

@property (nonatomic, strong) NSString* url;    //  image url

@property (nonatomic, assign) float width;      //  image width

@property (nonatomic, assign) float height;     //  image height

@end

//  原生广告对象
@interface MARAdNativeData : NSObject

@property (nonatomic, strong) NSString* title;          //  广告标题

@property (nonatomic, strong) NSString* descrption;    //  描述

@property (nonatomic, strong) NSString* source;         //  来源

@property (nonatomic, strong) MARAdImageInfo* icon;     //  icon

@property (nonatomic, strong) NSMutableArray<MARAdImageInfo *> *imageList;      //  图片列表

@property (nonatomic, assign) int interactionType;      //  1、点击；2、浏览器中打开；3、在App中打开；4、下载应用 5、拨打电话 其他：未知

@property (nonatomic, assign) int imageMode;            //  2: 小图；3: 大图；4: 组图；5:视频；其他：其他类型

@property (nonatomic, assign) int patternType;          //  广告样式

@end

//  闪屏广告事件回调
@protocol MARAdSplashDelegate

- (void) MARAdSplashDidFailed: (MARAdErrorCode)code withMessage: (NSString *)message;
- (void) MARAdSplashDidLoaded;
- (void) MARAdSplashDidShow;
- (void) MARAdSplashDidClicked;
- (void) MARAdSplashDidClosed;
- (void) MARAdSplashDidSkipped;

@end

//  插屏广告事件回调
@protocol MARAdPopupDelegate

- (void) MARAdPopupDidFailed: (MARAdErrorCode)code withMessage: (NSString *)message;
- (void) MARAdPopupDidLoaded;
- (void) MARAdPopupDidShow;
- (void) MARAdPopupDidClicked;
- (void) MARAdPopupDidClosed;
- (void) MARAdPopupDidSkipped;

@end

//  Banner 广告事件回调
@protocol MARAdBannerDelegate

- (void) MARAdBannerDidFailed: (MARAdErrorCode)code withMessage: (NSString *)message;
- (void) MARAdBannerDidLoaded;
- (void) MARAdBannerDidShow;
- (void) MARAdBannerDidClicked;
- (void) MARAdBannerDidClosed;
- (void) MARAdBannerDidSkipped;

@end

//  激励视频广告事件回调
@protocol MARAdRewardedDelegate

- (void) MARAdRewardedDidFailed: (MARAdErrorCode)code withMessage: (NSString *)message;
- (void) MARAdRewardedDidLoaded;
- (void) MARAdRewardedDidShow;
- (void) MARAdRewardedDidClicked;
- (void) MARAdRewardedDidClosed;
- (void) MARAdRewardedDidSkipped;
- (void) MARAdRewardedDidFinished: (NSString *)itemName itemNum:(int)itemNum;

@end

//  原生广告事件回调
@protocol MARAdNativeDelegate

- (void) MARAdNativeDidFailed: (MARAdErrorCode)code withMesage: (NSString *)message;
- (void) MARAdNativeDidLoaded: (MARAdNativeData *)adData;
- (void) MARAdNativeDidShow;
- (void) MARAdNativeDidClicked;
- (void) MARAdNativeDidClosed;
- (void) MARAdNativeDidSkipped;

@end

//  广告接口
@protocol IMARAd

- (void) showSplashAd: (UIViewController *)controller delegate: (id<MARAdSplashDelegate>)delegate;

- (void) showPopupAd: (UIViewController *)controller delegate: (id<MARAdPopupDelegate>)delegate;

- (void) showBannerAd: (UIViewController *)controller view: (UIView *)container delegate: (id<MARAdBannerDelegate>)delegate;

- (void) showBannerAd: (UIViewController *)controller delegate: (id<MARAdBannerDelegate>)delegate;

- (void) loadRewardVideoAd: (UIViewController *)controller itemName: (NSString *)itemName itemNum: (int)num delegate: (id<MARAdRewardedDelegate>)delegate;

- (void) showRewardVideoAdForLoad: (UIViewController *)controller;

- (void) showRewardVideoAd: (UIViewController *)controller itemName: (NSString *)itemName itemNum: (int)num delegate: (id<MARAdRewardedDelegate>)delegate;

@optional

- (void) loadNativeAd: (UIViewController *)controller delegate: (id<MARAdNativeDelegate>)delegate;
- (void) bindAdToView: (UIViewController *)controller containerView: (__kindof UIView *)containerView withClickableViews: (NSArray<__kindof UIView *> *_Nullable)clickableViews;
@end


NS_ASSUME_NONNULL_BEGIN

@interface MARAd : NSObject<IMARAd>

/// 广告接口实现
@property (nonatomic, strong) NSObject<IMARAd>* ad;

/// 获取MARAd的单例
+(instancetype) sharedInstance;

@end

NS_ASSUME_NONNULL_END
