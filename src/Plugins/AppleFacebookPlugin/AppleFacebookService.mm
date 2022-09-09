#include "AppleFacebookService.h"

#include "Environment/Apple/AppleUtils.h"

#include "Kernel/Logger.h"

#import <Foundation/Foundation.h>

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
        , m_shareDelegate( nullptr )
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
        NSString * strPicture = [NSString stringWithUTF8String:picture];
        NSString * strlink = [NSString stringWithUTF8String:link];
        
        if(strlink.length<=0 && strPicture.length<=0)
        {
            if( m_provider != nullptr )
            {
                m_provider->onFacebookShareError( "empty data" );
            }
            return;
        }
        
        if( m_shareDelegate != nullptr)
        {
            LOGGER_ERROR("Facebook  m_shareDelegate has nullptr -> not called share Dialog");
            return;
        }
        
        if(strPicture.length<=0)
        {
            dispatch_async(dispatch_get_main_queue(), ^{
                
                FBSDKShareLinkContent* content = [FBSDKShareLinkContent alloc];
                content.contentURL = [NSURL URLWithString:strlink];
                
                FBSDKShareDialog * const dialog = [FBSDKShareDialog dialogWithViewController:[UIApplication sharedApplication].delegate.window.rootViewController withContent:content delegate:m_shareDelegate];
                
                [dialog show];
            });

            return;
        }
        else
        {
            
            NSURL* url = [NSURL URLWithString:strPicture];
            if( url == nullptr)
            {
                LOGGER_ERROR("Facebook  picture not convert to NSURL");
                if( m_provider != nullptr )
                {
                    m_provider->onFacebookShareError( "error picture to NSURL" );
                }
                return;
            }
            
            NSData *data = [NSData dataWithContentsOfURL: url];
            if( data == nullptr)
            {
                LOGGER_ERROR("Facebook  picture not convert to NSData");
                if( m_provider != nullptr )
                {
                    m_provider->onFacebookShareError( "empty NSURL to NSData" );
                }
                return;
            }
            
            UIImage *img = [UIImage imageWithData: data];
            if( img == nullptr)
            {
                LOGGER_ERROR("Facebook  picture not convert to UIImage");
                if( m_provider != nullptr )
                {
                    m_provider->onFacebookShareError( "empty NSData to UIImage" );
                }
                return;
            }
            
            dispatch_async(dispatch_get_main_queue(), ^{
                
                FBSDKSharePhoto *sharePhoto = [[FBSDKSharePhoto alloc] initWithImage: img isUserGenerated:true];
                
                FBSDKShareMediaContent *content = [FBSDKShareMediaContent alloc];
                content.contentURL = [NSURL URLWithString:strlink];
                content.media =  @[sharePhoto];
                
                FBSDKShareDialog * const dialog = [FBSDKShareDialog dialogWithViewController:[UIApplication sharedApplication].delegate.window.rootViewController withContent:content delegate:m_shareDelegate];
                
                [dialog show];
                
            });
        }
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
        m_shareDelegate = [[AppleFacebookShareDelegate alloc]initWithService: this];
        
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
