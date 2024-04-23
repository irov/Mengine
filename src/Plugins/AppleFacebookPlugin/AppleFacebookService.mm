#include "AppleFacebookService.h"

#import "Environment/Apple/AppleErrorHelper.h"
#import "Environment/Apple/AppleDetail.h"
#import "Environment/iOS/iOSApplication.h"
#import "Environment/iOS/iOSDetail.h"

#include "Kernel/Logger.h"
#include "Kernel/ThreadHelper.h"

#include "Config/StdString.h"

////////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleFacebookService, Mengine::AppleFacebookService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleFacebookService::AppleFacebookService()
        : m_isProcessed( false )
        , m_loginManager( nil )
        , m_userID( nullptr )
        , m_imageURL( nil )
        , m_shareDelegate( nil )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleFacebookService::~AppleFacebookService()
    {
    }
    /////////////////////////////////////////////////////////////////////////////
    bool AppleFacebookService::_initializeService()
    {
        m_loginManager = [[FBSDKLoginManager alloc] init];
        m_shareDelegate = [[AppleFacebookShareDelegate alloc]initWithService: this];
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleFacebookService::_finalizeService()
    {
        m_provider = nullptr;
        
        m_loginManager = nil;
        m_shareDelegate = nil;
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
    void AppleFacebookService::login( const VectorConstString & _permissions )
    {
        if( m_isProcessed == true )
        {
            return;
        }
        
        m_isProcessed = true;
        
        UIViewController * rootViewController = Helper::iOSGetRootViewController();
        
        NSArray<NSString *> * permissions_ns = Helper::AppleGetNSArrayFromVectorConstString( _permissions );
        
        NSString * sessionId = [iOSApplication.sharedInstance getSessionId];
        
        FBSDKLoginConfiguration *configuration = [[FBSDKLoginConfiguration alloc] initWithPermissions:permissions_ns
                                                                                             tracking:FBSDKLoginTrackingLimited
                                                                                                nonce:sessionId];
        
        [m_loginManager logInFromViewController:rootViewController configuration:configuration completion:^(FBSDKLoginManagerLoginResult * _Nullable result, NSError * _Nullable error) {
            AppleFacebookProviderInterfacePtr copy_provider = m_provider;
            
            if( error != nullptr )
            {
                LOGGER_ERROR( "[AppleFacebook] login error: '%s'"
                   , Helper::AppleGetMessageFromNSError(error).c_str()
                );
                
                m_isProcessed = false;
                
                if( copy_provider != nullptr )
                {
                    Helper::dispatchMainThreadEvent([copy_provider, error](){
                        copy_provider->onFacebookError( -2, [NSString stringWithFormat:@"%@", error].UTF8String);
                    });
                }
                
                return;
            }
            
            if( result.isCancelled == YES )
            {
                LOGGER_ERROR( "[AppleFacebook] login cancel" );
                
                m_isProcessed = false;
                
                if( copy_provider != nullptr )
                {
                    Helper::dispatchMainThreadEvent([copy_provider, error](){
                        copy_provider->onFacebookLoginCancel();
                    });
                }
                
                return;
            }
            
            FBSDKProfile * profile = [FBSDKProfile currentProfile];
                            
            m_userID = profile.userID;
                
            if( profile.imageURL != nil )
            {
                NSURL * imageURL = profile.imageURL.absoluteURL;
                
                if( imageURL != nullptr && imageURL.absoluteString )
                {
                    m_imageURL = imageURL.absoluteString;
                }
            }
            
            m_isProcessed = false;
                
            if( copy_provider != nullptr )
            {
                Helper::dispatchMainThreadEvent([copy_provider](){
                    FBSDKAuthenticationToken * authenticationToken = [FBSDKAuthenticationToken currentAuthenticationToken];
                    
                    if( authenticationToken != nil )
                    {
                        NSString * authenticationTokenString = authenticationToken.tokenString;
                        const Char * authenticationTokenString_str = [authenticationTokenString UTF8String];
                        
                        copy_provider->onFacebookLoginSuccess( authenticationTokenString_str );
                    }
                    else
                    {
                        Helper::dispatchMainThreadEvent([copy_provider](){
                            copy_provider->onFacebookError( -1, "token == null" );
                        });
                    }
                });
            }
        }];
    }
    /////////////////////////////////////////////////////////////////////////////
    void AppleFacebookService::logout()
    {
        [m_loginManager logOut];
        
        [FBSDKAuthenticationToken setCurrentAuthenticationToken:nil];
        [FBSDKProfile setCurrentProfile:nil];
    }
    /////////////////////////////////////////////////////////////////////////////
    bool AppleFacebookService::isLoggedIn() const
    {
        FBSDKAuthenticationToken * authenticationToken = [FBSDKAuthenticationToken currentAuthenticationToken];
        
        if( authenticationToken == nil )
        {
            return false;
        }
        
        return true;
    }
    /////////////////////////////////////////////////////////////////////////////
    bool AppleFacebookService::getAccessToken( Char * const _token, size_t _capacity ) const
    {
        FBSDKAuthenticationToken * authenticationToken = [FBSDKAuthenticationToken currentAuthenticationToken];
        
        if( authenticationToken == nil )
        {
            return false;
        }
        
        const Char * token_str = authenticationToken.tokenString.UTF8String;
        
        MENGINE_STRNCPY( _token, token_str, _capacity );
        
        return true;
    }
    /////////////////////////////////////////////////////////////////////////////
    bool AppleFacebookService::getUserId( Char * const _userId, size_t _capacity ) const
    {
        FBSDKProfile * profile = [FBSDKProfile currentProfile];
        
        if( profile == nil )
        {
            return false;
        }
        
        NSString * userId = [profile userID];
        
        const Char * userId_str = userId.UTF8String;
        
        MENGINE_STRNCPY( _userId, userId_str, _capacity );
        
        return true;
    }
    /////////////////////////////////////////////////////////////////////////////
    void AppleFacebookService::shareLink( const Char * link, const Char * picture )
    {
        AppleFacebookProviderInterfacePtr copy_provider = m_provider;
        
        NSString * strPicture = [NSString stringWithUTF8String:picture];
        NSString * strlink = [NSString stringWithUTF8String:link];
        
        if( strlink.length <= 0 && strPicture.length <= 0 )
        {
            if( copy_provider != nullptr )
            {
                Helper::dispatchMainThreadEvent([copy_provider](){
                    copy_provider->onFacebookShareError( -1, "empty data" );
                });
            }

            return;
        }
        
        if( m_shareDelegate != nullptr )
        {
            LOGGER_ERROR("Facebook m_shareDelegate has nullptr -> not called share Dialog");

            return;
        }
        
        if( strPicture.length <= 0 )
        {
            dispatch_async(dispatch_get_main_queue(), ^{
                FBSDKShareLinkContent* content = [FBSDKShareLinkContent alloc];
                content.contentURL = [NSURL URLWithString:strlink];
                
                UIViewController * rootViewController = Helper::iOSGetRootViewController();
                
                FBSDKShareDialog * dialog = [FBSDKShareDialog dialogWithViewController:rootViewController withContent:content delegate:m_shareDelegate];
                
                [dialog show];
            });

            return;
        }
        else
        {            
            NSURL * url = [NSURL URLWithString:strPicture];

            if( url == nil )
            {
                LOGGER_ERROR("Facebook picture not convert to NSURL");

                if( copy_provider != nullptr )
                {
                    Helper::dispatchMainThreadEvent([copy_provider](){
                        copy_provider->onFacebookShareError( -2, "error picture to NSURL" );
                    });
                }

                return;
            }
            
            NSData * data = [NSData dataWithContentsOfURL: url];

            if( data == nil )
            {
                LOGGER_ERROR("Facebook picture not convert to NSData");

                if( copy_provider != nullptr )
                {
                    Helper::dispatchMainThreadEvent([copy_provider](){
                        copy_provider->onFacebookShareError( -3, "empty NSURL to NSData" );
                    });
                }

                return;
            }
            
            UIImage * img = [UIImage imageWithData: data];

            if( img == nil )
            {
                LOGGER_ERROR("Facebook picture not convert to UIImage");

                if( copy_provider != nullptr )
                {
                    Helper::dispatchMainThreadEvent([copy_provider](){
                        copy_provider->onFacebookShareError( -4, "empty NSData to UIImage" );
                    });
                }
                
                return;
            }
            
            dispatch_async(dispatch_get_main_queue(), ^{
                FBSDKSharePhoto *sharePhoto = [[FBSDKSharePhoto alloc] initWithImage: img isUserGenerated:true];
                
                FBSDKShareMediaContent *content = [FBSDKShareMediaContent alloc];
                content.contentURL = [NSURL URLWithString:strlink];
                content.media =  @[sharePhoto];
                
                UIViewController * rootViewController = Helper::iOSGetRootViewController();
                
                FBSDKShareDialog * const dialog = [FBSDKShareDialog dialogWithViewController:rootViewController withContent:content delegate:m_shareDelegate];
                
                [dialog show];
            });
        }
    }
    /////////////////////////////////////////////////////////////////////////////
    void AppleFacebookService::getProfilePictureLink()
    {
        NSString * copy_userID = m_userID;
        NSString * copy_imageURL = m_imageURL;
        AppleFacebookProviderInterfacePtr copy_provider = m_provider;
        
        if( copy_imageURL != nil )
        {
            if( copy_provider != nil )
            {
                Helper::dispatchMainThreadEvent([copy_provider, copy_userID, copy_imageURL](){
                    const Char * userId = copy_userID != nil ? copy_userID.UTF8String : nullptr;
                    const Char * imageURL = copy_imageURL.UTF8String;
                    
                    copy_provider->onFacebookProfilePictureLinkGet( userId, true, imageURL );
                });
            }
            
            return;
        }
        
        
        [FBSDKProfile loadCurrentProfileWithCompletion:^(FBSDKProfile * _Nullable profile, NSError * _Nullable error) {
            if( error != nil )
            {
                LOGGER_ERROR("[AppleFacebook] get picture error: '%s'"
                   , Helper::AppleGetMessageFromNSError(error).c_str()
                );
                
                if( copy_provider != nullptr )
                {
                    Helper::dispatchMainThreadEvent([copy_provider, error](){
                        NSInteger error_code = error.code;
                        const Char * error_message = error.localizedDescription.UTF8String;
                        
                        copy_provider->onFacebookError( (int32_t)error_code, error_message );
                    });
                }
                
                return;
            }
            
            if( profile != nil && profile.imageURL != nil )
            {
                if( profile.imageURL.absoluteURL != nil )
                {
                    if( copy_provider != nullptr )
                    {
                        Helper::dispatchMainThreadEvent([copy_provider, profile](){
                            const Char * userId_str = profile.userID.UTF8String;
                            
                            NSString * imageURL = profile.imageURL.absoluteURL.absoluteString;
                            const Char * imageURL_str = imageURL.UTF8String;
                            
                            copy_provider->onFacebookProfilePictureLinkGet( userId_str, true, imageURL_str );
                        });
                    }
                    
                    return;
                }
            }
            else
            {
                if( copy_provider != nullptr )
                {
                    Helper::dispatchMainThreadEvent([copy_provider](){
                        copy_provider->onFacebookProfilePictureLinkGet( "", false, "" );
                    });
                }
            }
        }];
    }
    //////////////////////////////////////////////////////////////////////////
}
