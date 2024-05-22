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
        : m_loginManager( nil )
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
    bool AppleFacebookService::login( bool _limited, const VectorConstString & _permissions )
    {        
        UIViewController * rootViewController = Helper::iOSGetRootViewController();
        
        NSArray<NSString *> * permissions_ns = Helper::AppleGetNSArrayFromVectorConstString( _permissions );
        
        NSString * sessionId = [iOSApplication.sharedInstance getSessionId];
        
        FBSDKLoginConfiguration * configuration = [[FBSDKLoginConfiguration alloc] initWithPermissions:permissions_ns
                                                                                              tracking:(_limited == true ? FBSDKLoginTrackingLimited : FBSDKLoginTrackingEnabled)
                                                                                                 nonce:sessionId];
        
        [m_loginManager logInFromViewController:rootViewController configuration:configuration completion:^(FBSDKLoginManagerLoginResult * _Nullable result, NSError * _Nullable error) {
            AppleFacebookProviderInterfacePtr copy_provider = m_provider;
            
            if( error != nullptr )
            {
                LOGGER_ERROR( "[AppleFacebook] login error: '%s'"
                   , Helper::AppleGetMessageFromNSError(error).c_str()
                );
                
                if( copy_provider != nullptr )
                {
                    Helper::dispatchMainThreadEvent( [copy_provider, error]() {
                        copy_provider->onFacebookError( -2, [NSString stringWithFormat:@"%@", error].UTF8String);
                    });
                }
                
                return;
            }
            
            if( result.isCancelled == YES )
            {
                LOGGER_ERROR( "[AppleFacebook] login cancel" );
                
                if( copy_provider != nullptr )
                {
                    Helper::dispatchMainThreadEvent( [copy_provider, error]() {
                        copy_provider->onFacebookLoginCancel();
                    });
                }
                
                return;
            }
            
            LOGGER_MESSAGE( "[AppleFacebook] login successful" );
                
            if( copy_provider != nullptr )
            {
                Helper::dispatchMainThreadEvent( [copy_provider]() {
                    Params params;
                    
                    FBSDKProfile * profile = [FBSDKProfile currentProfile];
                    
                    if( profile != nil )
                    {
                        NSString * userID = profile.userID;
                        const Char * userID_str = [userID UTF8String];
                        
                        params.emplace( STRINGIZE_STRING_LOCAL("profile.userID"), userID_str );
                        
                        if( profile.imageURL != nil )
                        {
                            NSURL * imageURL = profile.imageURL.absoluteURL;
                            
                            if( imageURL != nil && imageURL.absoluteString != nil )
                            {
                                const Char * imageURL_str = [imageURL.absoluteString UTF8String];
                                
                                params.emplace( STRINGIZE_STRING_LOCAL("profile.imageURL"), imageURL_str );
                            }
                        }
                    }
                    
                    FBSDKAuthenticationToken * authenticationToken = [FBSDKAuthenticationToken currentAuthenticationToken];
                    
                    if( authenticationToken != nil )
                    {
                        NSString * authenticationTokenString = authenticationToken.tokenString;
                        const Char * authenticationTokenString_str = [authenticationTokenString UTF8String];
                        
                        params.emplace( STRINGIZE_STRING_LOCAL("authentication.token"), authenticationTokenString_str );
                    }
                    
                    FBSDKAccessToken * accessToken = [FBSDKAccessToken currentAccessToken];
                    
                    if( accessToken != nil )
                    {
                        NSString * accessTokenString = accessToken.tokenString;
                        const Char * accessTokenString_str = [accessTokenString UTF8String];
                        
                        params.emplace( STRINGIZE_STRING_LOCAL("access.token"), accessTokenString_str );
                    }
                                         
                    copy_provider->onFacebookLoginSuccess( params );
                });
            }
        }];
        
        return true;
    }
    /////////////////////////////////////////////////////////////////////////////
    void AppleFacebookService::logout()
    {
        [m_loginManager logOut];
        
        [FBSDKAuthenticationToken setCurrentAuthenticationToken:nil];
        [FBSDKProfile setCurrentProfile:nil];
    }
    /////////////////////////////////////////////////////////////////////////////
    bool AppleFacebookService::isAccessSuccess() const
    {
        FBSDKAccessToken * accessToken = [FBSDKAccessToken currentAccessToken];
        
        if( accessToken == nil )
        {
            return false;
        }
        
        return true;
    }
    /////////////////////////////////////////////////////////////////////////////
    bool AppleFacebookService::isAuthenticationSuccess() const
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
        FBSDKAccessToken * accessToken = [FBSDKAccessToken currentAccessToken];
        
        if( accessToken == nil )
        {
            return false;
        }
        
        const Char * token_str = accessToken.tokenString.UTF8String;
        
        MENGINE_STRNCPY( _token, token_str, _capacity );
        
        return true;
    }
    /////////////////////////////////////////////////////////////////////////////
    bool AppleFacebookService::getAuthenticationToken( Char * const _token, size_t _capacity ) const
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
            LOGGER_ERROR("Facebook link and picture is empty");

            if( copy_provider != nullptr )
            {
                Helper::dispatchMainThreadEvent( [copy_provider]() {
                    copy_provider->onFacebookShareError( -1, "empty data" );
                });
            }

            return;
        }
        
        if( m_shareDelegate != nullptr )
        {
            LOGGER_ERROR("Facebook m_shareDelegate has nullptr -> not called share Dialog");

            if( copy_provider != nullptr )
            {
                Helper::dispatchMainThreadEvent( [copy_provider]() {
                    copy_provider->onFacebookShareError( -2, "share delegate nullptr" );
                });
            }

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
            
        NSURL * url = [NSURL URLWithString:strPicture];

        if( url == nil )
        {
            LOGGER_ERROR("Facebook picture not convert to NSURL");

            if( copy_provider != nullptr )
            {
                Helper::dispatchMainThreadEvent( [copy_provider]() {
                    copy_provider->onFacebookShareError( -3, "error picture to NSURL" );
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
                Helper::dispatchMainThreadEvent( [copy_provider]() {
                    copy_provider->onFacebookShareError( -4, "empty NSURL to NSData" );
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
                Helper::dispatchMainThreadEvent( [copy_provider]() {
                    copy_provider->onFacebookShareError( -5, "empty NSData to UIImage" );
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
    /////////////////////////////////////////////////////////////////////////////
    void AppleFacebookService::getProfilePictureLink()
    {
        AppleFacebookProviderInterfacePtr copy_provider = m_provider;

        FBSDKProfile * profile = [FBSDKProfile currentProfile];
        
        if( profile != nil )
        {
            if( copy_provider != nullptr )
            {
                Helper::dispatchMainThreadEvent( [copy_provider, profile]() {
                    NSString * userID = profile.userID;
                
                    const Char * userID_str = userID != nil ? userID.UTF8String : nullptr;
                    const Char * imageURL_str = "";
                
                    if( profile.imageURL != nil )
                    {
                        NSURL * imageURL = profile.imageURL.absoluteURL;
                    
                        if( imageURL != nullptr && imageURL.absoluteString )
                        {
                            imageURL_str = [imageURL.absoluteString UTF8String];
                        }
                    }
                
                    copy_provider->onFacebookProfilePictureLinkGetSuccess( userID_str, true, imageURL_str );
                });
            }
            
            return;
        }
        
        
        [FBSDKProfile loadCurrentProfileWithCompletion:^(FBSDKProfile * _Nullable _profile, NSError * _Nullable _error) {
            if( _error != nil )
            {
                LOGGER_ERROR("[AppleFacebook] get picture error: '%s'"
                   , Helper::AppleGetMessageFromNSError(_error).c_str()
                );
                
                if( copy_provider != nullptr )
                {
                    Helper::dispatchMainThreadEvent( [copy_provider, _error]() {
                        NSInteger error_code = _error.code;
                        const Char * error_message = _error.localizedDescription.UTF8String;
                    
                        copy_provider->onFacebookProfilePictureLinkGetError( (int32_t)error_code, error_message );
                    });
                }
                
                return;
            }
            
            if( _profile == nil )
            {
                if( copy_provider != nullptr )
                {
                    Helper::dispatchMainThreadEvent( [copy_provider]() {
                        copy_provider->onFacebookProfilePictureLinkGetError( -1, "profile is nil" );
                    });
                }
                
                return;
            }
            
            if( _profile.imageURL == nil )
            {
                if( copy_provider != nullptr )
                {
                    Helper::dispatchMainThreadEvent( [copy_provider]() {
                        copy_provider->onFacebookProfilePictureLinkGetError( -2, "imageURL is nil" );
                    });
                }
                
                return;
            }
            
            if( _profile.imageURL.absoluteURL == nil )
            {
                if( copy_provider != nullptr )
                {
                    Helper::dispatchMainThreadEvent( [copy_provider]() {
                        copy_provider->onFacebookProfilePictureLinkGetError( -3, "absoluteURL is nil" );
                    });
                }
                
                return;
            }
            
            if( copy_provider != nullptr )
            {
                Helper::dispatchMainThreadEvent( [copy_provider, _profile]() {
                    const Char * userID_str = _profile.userID.UTF8String;
                    
                    NSString * imageURL = _profile.imageURL.absoluteURL.absoluteString;
                    const Char * imageURL_str = imageURL.UTF8String;
                
                    copy_provider->onFacebookProfilePictureLinkGetSuccess( userID_str, true, imageURL_str );
                });
            }
        }];
    }
    //////////////////////////////////////////////////////////////////////////
}
