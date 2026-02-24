#import "AppleFacebookShareDelegate.h"

#import "Environment/Apple/AppleDetail.h"

@implementation AppleFacebookShareDelegate

#pragma mark -

@synthesize m_facebook;

- (instancetype _Nonnull) initWithFacebook:(id<AppleFacebookInterface> _Nonnull)facebook {
    self = [super init];
    
    self.m_facebook = facebook;
    
    return self;
}

- (void) sharer:(id <FBSDKSharing> _Nonnull) sharer didCompleteWithResults:(NSDictionary<NSString *, id> * _Nonnull) results {
    id<AppleFacebookProviderInterface> provider = [self.m_facebook getProvider];

    if( provider == nil )
    {
        return;
    }

    NSString * message = sharer.shareContent == nil ? @"" : sharer.shareContent.placeID;
    if( message == nil )
    {
        message = @"";
    }

    [AppleDetail addMainQueueOperation:^{
        [provider onFacebookShareSuccess:message];
    }];
}

- (void) sharer:(id <FBSDKSharing> _Nonnull) sharer didFailWithError:(NSError * _Nonnull) error {
    (void)sharer;

    id<AppleFacebookProviderInterface> provider = [self.m_facebook getProvider];

    if( provider == nil )
    {
        return;
    }

    NSInteger code = error.code;
    NSString * message = error.description;

    [AppleDetail addMainQueueOperation:^{
        [provider onFacebookShareError:code message:message];
    }];
}

- (void) sharerDidCancel:(id <FBSDKSharing> _Nonnull) sharer {
    (void)sharer;

    id<AppleFacebookProviderInterface> provider = [self.m_facebook getProvider];

    if( provider == nil )
    {
        return;
    }

    [AppleDetail addMainQueueOperation:^{
        [provider onFacebookShareCancel];
    }];
}

@end
