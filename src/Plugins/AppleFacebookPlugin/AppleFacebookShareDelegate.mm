#import "AppleFacebookShareDelegate.h"

@implementation AppleFacebookShareDelegate

#pragma mark -

@synthesize m_service;

- (instancetype _Nonnull) initWithService: (Mengine::AppleFacebookServiceInterface* _Nonnull) service {
    self = [super init];
    
    self.m_service = service;
    
    return self;
}

- (void) sharer:(id <FBSDKSharing> _Nonnull) sharer didCompleteWithResults:(NSDictionary<NSString *, id> * _Nonnull) results {
    const char * message = sharer.shareContent == nullptr ? "" : sharer.shareContent.placeID.UTF8String;
    
    const Mengine::AppleFacebookProviderInterfacePtr & provider = self.m_service->getProvider();
    
    provider->onFacebookShareSuccess( message );
}

- (void) sharer:(id <FBSDKSharing> _Nonnull) sharer didFailWithError:(NSError * _Nonnull) error {
    NSInteger code = error.code;
    const char * message = error.localizedDescription.UTF8String;
    
    const Mengine::AppleFacebookProviderInterfacePtr & provider = self.m_service->getProvider();
    
    provider->onFacebookShareError( (int32_t)code, message );
}

- (void) sharerDidCancel:(id <FBSDKSharing> _Nonnull) sharer {
    
    const Mengine::AppleFacebookProviderInterfacePtr & provider = self.m_service->getProvider();
    
    provider->onFacebookShareCancel();
}

@end
