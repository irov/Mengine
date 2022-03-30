//
//  MARUser.h
//  MARSDKCore
//
//  Created by js wu on 2021/6/28.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

//MARUser 账号登录相关接口
@protocol IMARUser

- (void) login;
- (void) logout;
- (void) switchAccount;

- (BOOL) hasAccountCenter;

- (BOOL) hasRealNameQuery;
- (BOOL) hasRealNameRegister;
- (void) realNameQuery;
- (void) realNameRegister;

@optional
- (void) loginCustom:(NSString*)customData;
- (void) showAccountCenter;
- (void) exitSDK;
@end

__attribute__ ((deprecated))
@protocol MARUser
@end
