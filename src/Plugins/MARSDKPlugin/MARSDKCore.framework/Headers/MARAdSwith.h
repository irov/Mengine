//
//  MARAdSwith.h
//  MARSDKCore
//
//  Created by 火星人 on 2022/3/11.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface MARAdSwith : NSObject

//banner 开关
@property (strong, nonatomic) NSString *bannerSwitch;
//插屏开关
@property (strong, nonatomic) NSString *intersSwitch;
//开屏开关
@property (strong, nonatomic) NSString *splashSwitch;
//激励视频开关
@property (strong, nonatomic) NSString *videoSwitch;
//插屏视频开关
@property (strong, nonatomic) NSString *intersVideoSwitch;
//原生横屏开关
@property (strong, nonatomic) NSString *nativeBannerSwitch;
//原生插屏开关
@property (strong, nonatomic) NSString *nativeIntersSwitch;
//原生大图开关
@property (strong, nonatomic) NSString *nativeBigSwitch;



+(MARAdSwith*) sharedInstance;

@end

NS_ASSUME_NONNULL_END
