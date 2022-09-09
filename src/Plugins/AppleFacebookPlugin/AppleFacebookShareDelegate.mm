#import "AppleFacebookShareDelegate.h"

#include "Environment/Apple/AppleUtils.h"

#include "Kernel/Logger.h"

@implementation AppleFacebookShareDelegate

#pragma mark -

@synthesize m_service;

- (instancetype _Nonnull)initWithService: (Mengine::AppleFacebookServiceInterface* _Nonnull)service {
    self = [super init];
    
    self.m_service = service;
    
    return self;
}


- (void)sharer:(id <FBSDKSharing> _Nonnull)sharer didCompleteWithResults:(NSDictionary<NSString *, id> * _Nonnull)results
{
    const char* message = sharer.shareContent == nullptr ? "" : sharer.shareContent.placeID.UTF8String;
    
    self.m_service->getProvider()->onFacebookShareSuccess( message );
}


- (void)sharer:(id <FBSDKSharing> _Nonnull)sharer didFailWithError:(NSError * _Nonnull)error{
    
    self.m_service->getProvider()->onFacebookShareError(error.localizedDescription.UTF8String );
}


- (void)sharerDidCancel:(id <FBSDKSharing> _Nonnull)sharer{
    
   self.m_service->getProvider()->onFacebookShareCancel();
}

@end
