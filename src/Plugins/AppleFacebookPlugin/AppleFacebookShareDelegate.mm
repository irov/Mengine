#import "AppleFacebookShareDelegate.h"

#include "Kernel/ThreadHelper.h"

@implementation AppleFacebookShareDelegate

#pragma mark -

@synthesize m_service;

- (instancetype _Nonnull) initWithService: (Mengine::AppleFacebookServiceInterface* _Nonnull) service {
    self = [super init];
    
    self.m_service = service;
    
    return self;
}

- (void) sharer:(id <FBSDKSharing> _Nonnull) sharer didCompleteWithResults:(NSDictionary<NSString *, id> * _Nonnull) results {
    Mengine::AppleFacebookProviderInterfacePtr copy_provider = self.m_service->getProvider();
    
    Mengine::Helper::dispatchMainThreadEvent([copy_provider, sharer](){
        const char * message = sharer.shareContent == nullptr ? "" : sharer.shareContent.placeID.UTF8String;
        
        copy_provider->onFacebookShareSuccess( message );
    });
}

- (void) sharer:(id <FBSDKSharing> _Nonnull) sharer didFailWithError:(NSError * _Nonnull) error {
    Mengine::AppleFacebookProviderInterfacePtr copy_provider = self.m_service->getProvider();
    
    Mengine::Helper::dispatchMainThreadEvent([copy_provider, error](){
        NSInteger code = error.code;
        const char * message = [error.description UTF8String];
        
        copy_provider->onFacebookShareError( (int32_t)code, message );
    });
}

- (void) sharerDidCancel:(id <FBSDKSharing> _Nonnull) sharer {
    Mengine::AppleFacebookProviderInterfacePtr copy_provider = self.m_service->getProvider();
    
    Mengine::Helper::dispatchMainThreadEvent([copy_provider](){
        copy_provider->onFacebookShareCancel();
    });
}

@end
