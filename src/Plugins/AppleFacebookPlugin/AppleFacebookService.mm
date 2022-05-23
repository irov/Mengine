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
    AppleFacebookService::AppleFacebookService()
        : m_isProcessed( false )
        , m_loginManager( nullptr )
        , m_userID( nullptr )
        , m_imageURL( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleFacebookService::~AppleFacebookService()
    {
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
    void AppleFacebookService::login()
    {
        if( m_isProcessed == true )
        {
            return;
        }
        
        m_isProcessed = true;
        
        [m_loginManager logInWithPermissions:@[@"public_profile"] fromViewController:[UIApplication sharedApplication].delegate.window.rootViewController handler:^(FBSDKLoginManagerLoginResult * _Nullable result, NSError * _Nullable error) {
            if( error != nullptr )
            {
                LOGGER_ERROR("[AppleFacebook] login error: '%s'"
                   , Helper::AppleGetMessageFromNSError(error).c_str()
                );
                
                m_isProcessed = false;
                
                if( m_provider != nullptr )
                {
                    m_provider->onFacebookError([NSString stringWithFormat:@"%@", error].UTF8String);
                }
                
                return;
            }
            
            if( result.isCancelled == true )
            {
                m_isProcessed = false;
                
                return;
            }
                
            m_userID = FBSDKProfile.currentProfile.userID;
                
            NSString * m_idTokenString = FBSDKAuthenticationToken.currentAuthenticationToken.tokenString;

            if( FBSDKProfile.currentProfile.imageURL )
            {
                NSURL * imageURL = FBSDKProfile.currentProfile.imageURL.absoluteURL;
                
                if( imageURL != nullptr && imageURL.absoluteString )
                {
                    m_imageURL = imageURL.absoluteString;
                }
            }
            
            m_isProcessed = false;
                
            if( m_provider != nullptr )
            {
                if( m_idTokenString )
                {
                    m_provider->onFacebookLoginSuccess( m_idTokenString.UTF8String );
                }
                else
                {
                    m_provider->onFacebookError( "token == null" );
                }
            }
        }];
    }
    /////////////////////////////////////////////////////////////////////////////
    void AppleFacebookService::logout()
    {
        [m_loginManager logOut];
    }
    /////////////////////////////////////////////////////////////////////////////
    bool AppleFacebookService::isLoggedIn() const
    {
        return FBSDKAuthenticationToken.currentAuthenticationToken != nullptr;
    }
    /////////////////////////////////////////////////////////////////////////////
    const Char * AppleFacebookService::getAccessToken() const
    {
        if( this->isLoggedIn() == false )
        {
            return nullptr;
        }
        
        return FBSDKAccessToken.currentAccessToken.tokenString.UTF8String;
    }
    /////////////////////////////////////////////////////////////////////////////
    void AppleFacebookService::shareLink( const Char * link, const Char * picture )
    {
        [AppleFacebookHelper ShareImageLinkWithLink:[NSString stringWithUTF8String:link]  image:[NSString stringWithUTF8String:picture] cb:^(NSInteger _code, NSString * _Nullable data) {
            if( m_provider != nullptr )
            {
                switch( _code )
                {
                    case 0:
                    {
                        m_provider->onFacebookShareSuccess( data == nullptr ? "" : data.UTF8String );
                    }break;
                    case 1:
                    {
                        m_provider->onFacebookShareError( data == nullptr ? "" : data.UTF8String );
                    }break;
                    case 2:
                    {
                        m_provider->onFacebookShareCancel();
                    }break;
                }
            }
        }];
    }
    /////////////////////////////////////////////////////////////////////////////
    void AppleFacebookService::getProfilePictureLink()
    {
        if( m_imageURL != nullptr )
        {
            if( m_provider != nullptr )
            {
                const Char * userId = m_userID != nullptr ? m_userID.UTF8String : nullptr;
                
                m_provider->onFacebookProfilePictureLinkGet( userId, true, m_imageURL.UTF8String );
            }
            
            return;
        }
        
        
        [FBSDKProfile loadCurrentProfileWithCompletion:^(FBSDKProfile * _Nullable profile, NSError * _Nullable error) {
            if( error != nullptr )
            {
                LOGGER_ERROR("[AppleFacebook] get picture error: '%s'"
                   , Helper::AppleGetMessageFromNSError(error).c_str()
                );
                
                if( m_provider != nullptr )
                {
                    m_provider->onFacebookError([NSString stringWithFormat:@"%@", error].UTF8String);
                }
            }
            
            if( profile != nullptr && profile.imageURL )
            {
                if( profile.imageURL.absoluteURL )
                {
                    m_imageURL = profile.imageURL.absoluteURL.absoluteString;
                    
                    if( m_provider != nullptr )
                    {
                        m_provider->onFacebookProfilePictureLinkGet( profile.userID.UTF8String, true, m_imageURL.UTF8String );
                        
                        return;
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
}
