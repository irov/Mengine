#include "AppleFacebookService.h"

#include "Environment/Apple/AppleUtils.h"

#include "Kernel/Logger.h"

#import <Foundation/Foundation.h>

#import "AppleFacebookPlugin-Swift.h"
////////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleFacebookService, Mengine::AppleFacebookService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleFacebookService::AppleFacebookService():
        m_isProcessed(false),
        m_provider(nullptr),
        m_userID(nullptr),
        m_imageUrl(nullptr)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleFacebookService::~AppleFacebookService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFacebookService::login(){
        if(m_isProcessed) return;
        m_isProcessed = true;
        
        [m_loginManager logInWithPermissions:@[@"public_profile"] fromViewController:[UIApplication sharedApplication].delegate.window.rootViewController handler:^(FBSDKLoginManagerLoginResult * _Nullable result, NSError * _Nullable error) {
            if(error) {
                LOGGER_ERROR("[AppleFacebook] login error: '%s'"
                   , Helper::AppleGetMessageFromNSError(error).c_str()
                );
                
                if( m_provider != nullptr )
                {
                    m_provider->onFacebookError([NSString stringWithFormat:@"%@", error].UTF8String);
                }
            }
            if (!error && !result.isCancelled) {
                m_userID = FBSDKProfile.currentProfile.userID;
                NSString* m_idTokenString = FBSDKAuthenticationToken.currentAuthenticationToken.tokenString;

                if(FBSDKProfile.currentProfile.imageURL){
                    NSURL *imageURL = FBSDKProfile.currentProfile.imageURL.absoluteURL;
                    if(imageURL && imageURL.absoluteString){
                        m_imageUrl = imageURL.absoluteString;
                    }
                }
                
                if( m_provider != nullptr )
                {
                    if(m_idTokenString){
                        m_provider->onFacebookLoginSuccess(m_idTokenString.UTF8String);
                    }else{
                        m_provider->onFacebookError("token == null");
                    }
                }
            }
            m_isProcessed = false;
        }];
    }
    /////////////////////////////////////////////////////////////////////////////
    void AppleFacebookService::logout(){
        [m_loginManager logOut];
    }
    /////////////////////////////////////////////////////////////////////////////
    bool AppleFacebookService::isLoggedIn() const {
        return FBSDKAuthenticationToken.currentAuthenticationToken != nullptr;
    }
    /////////////////////////////////////////////////////////////////////////////
    const char* AppleFacebookService::getAccessToken() const{
        if(isLoggedIn()) return FBSDKAccessToken.currentAccessToken.tokenString.UTF8String;
        return NULL;
    }
    /////////////////////////////////////////////////////////////////////////////
    void AppleFacebookService::shareLink(const char* link,const char* picture){
    
        [AppleFacebookHelper ShareImageLinkWithLink:[NSString stringWithUTF8String:link]  image:[NSString stringWithUTF8String:picture] cb:^(NSInteger _code, NSString * _Nullable data) {
            
            if( m_provider != nullptr )
            {
                switch( _code )
                {
                    case 0:{
                        m_provider->onFacebookShareSuccess( data == nullptr ? "" : data.UTF8String);
                    }break;
                    case 1:{
                        m_provider->onFacebookShareError( data == nullptr ? "" : data.UTF8String);
                    }break;
                    case 2:{
                        m_provider->onFacebookShareCancel();
                    }break;
                }
            }
            
        }];
        
        
    }
    /////////////////////////////////////////////////////////////////////////////
    void AppleFacebookService::getProfilePictureLink() {
        if(m_imageUrl){
            if( m_provider != nullptr )
            {
                const char* userId = m_userID != nullptr ? m_userID.UTF8String : NULL;
                m_provider->onFacebookProfilePictureLinkGet(userId, true, m_imageUrl.UTF8String);
            }
            return;
        }
        
        
        [FBSDKProfile loadCurrentProfileWithCompletion:^(FBSDKProfile * _Nullable profile, NSError * _Nullable error) {
        
            if(error) {
                LOGGER_ERROR("[AppleFacebook] get picture error: '%s'"
                   , Helper::AppleGetMessageFromNSError(error).c_str()
                );
                
                if( m_provider != nullptr )
                {
                    m_provider->onFacebookError([NSString stringWithFormat:@"%@", error].UTF8String);
                }
            }
            if(profile && profile.imageURL){
                if(profile && profile.imageURL){
                    if(profile.imageURL.absoluteURL){
                        m_imageUrl = profile.imageURL.absoluteURL.absoluteString;
                        if( m_provider != nullptr )
                        {
                            m_provider->onFacebookProfilePictureLinkGet(profile.userID.UTF8String, true, m_imageUrl.UTF8String);
                            return;
                        }
                    }
                }
            }
            
            
            if( m_provider != nullptr )
            {
                m_provider->onFacebookProfilePictureLinkGet("", false, "");
            }
        }];
    }
    /////////////////////////////////////////////////////////////////////////////
    bool AppleFacebookService::_initializeService()
    {
        m_loginManager = [FBSDKLoginManager new];
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleFacebookService::_finalizeService()
    {
        m_provider = nullptr;
        
//        [m_loginManager release];
//        m_loginManager = nil;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFacebookService::setProvider( const AppleFacebookProviderInterfacePtr & _provider )
    {
        m_provider = _provider;
    }
    //////////////////////////////////////////////////////////////////////////
    const AppleFacebookProviderInterfacePtr & AppleFacebookService::getProvider() const
    {
        return m_provider;
    }
//////////////////////////////////////////////////////////////////////////
}
