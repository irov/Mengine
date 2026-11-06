#include "iOSFacebookScriptEmbedding.h"

#include "iOSFacebookInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Plugins/iOSNativePythonPlugin/iOSNativePythonTypeCast.h"
#include "Environment/Python/PythonDocument.h"

#import "Environment/Python/ApplePythonProvider.h"
#import "Environment/Apple/AppleDetail.h"

#import "iOSFacebookPlugin.h"

#include "Kernel/VectorConstString.h"

@interface PythoniOSFacebookProvider : ApplePythonProvider<iOSFacebookProviderInterface>
@end

@implementation PythoniOSFacebookProvider

- (void)onFacebookLoginSuccess:(NSDictionary<NSString *,NSString *> *)params {
    pybind::object py_cb = [self getMethod:@"oniOSFacebookLoginSuccess"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( params, self.m_args );
}

- (void)onFacebookLoginCancel {
    pybind::object py_cb = [self getMethod:@"oniOSFacebookLoginCancel"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( self.m_args );
}

- (void)onFacebookError:(NSInteger)code message:(NSString *)errorMessage {
    pybind::object py_cb = [self getMethod:@"oniOSFacebookError"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( code, errorMessage, self.m_args );
}

- (void)onFacebookShareSuccess:(NSString *)postId {
    pybind::object py_cb = [self getMethod:@"oniOSFacebookShareSuccess"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( postId, self.m_args );
}

- (void)onFacebookShareCancel {
    pybind::object py_cb = [self getMethod:@"oniOSFacebookShareCancel"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( self.m_args );
}

- (void)onFacebookShareError:(NSInteger)code message:(NSString *)errorMessage {
    pybind::object py_cb = [self getMethod:@"oniOSFacebookShareError"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( code, errorMessage, self.m_args );
}

- (void)onFacebookProfilePictureLinkGetSuccess:(NSString *)userId pictureURL:(NSString *)pictureURL {
    pybind::object py_cb = [self getMethod:@"oniOSFacebookProfilePictureLinkGetSuccess"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( userId, pictureURL, self.m_args );
}

- (void)onFacebookProfilePictureLinkGetError:(NSInteger)code message:(NSString *)errorMessage {
    pybind::object py_cb = [self getMethod:@"oniOSFacebookProfilePictureLinkGetError"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( code, errorMessage, self.m_args );
}

@end

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void iOSFacebook_setProvider( const pybind::dict & _cbs, const pybind::args & _args )
        {
            id<iOSFacebookProviderInterface> provider = [[PythoniOSFacebookProvider alloc] initWithCbs:_cbs args:_args];

            [[iOSFacebookPlugin sharedInstance] setProvider:provider];
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSFacebook_login( const VectorConstString & _permissions )
        {
            NSArray<NSString *> * permissions = [AppleDetail getNSArrayFromVectorConstString:_permissions];

            return [[iOSFacebookPlugin sharedInstance] login:permissions];
        }
        //////////////////////////////////////////////////////////////////////////
        static void iOSFacebook_logout()
        {
            [[iOSFacebookPlugin sharedInstance] logout];
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSFacebook_isLoggedIn()
        {
            bool loggedIn = [[iOSFacebookPlugin sharedInstance] isLoggedIn];

            return loggedIn;
        }
        //////////////////////////////////////////////////////////////////////////
        static NSString * iOSFacebook_getAccessToken()
        {
            NSString * accessToken = [[iOSFacebookPlugin sharedInstance] getAccessToken];

            return accessToken;
        }
        //////////////////////////////////////////////////////////////////////////
        static NSString * iOSFacebook_getUserId()
        {
            NSString * userId = [[iOSFacebookPlugin sharedInstance] getUserId];

            return userId;
        }
        //////////////////////////////////////////////////////////////////////////
        static NSDictionary * iOSFacebook_getUser()
        {
            NSDictionary * user = [[iOSFacebookPlugin sharedInstance] getUser];

            return user;
        }
        //////////////////////////////////////////////////////////////////////////
        static void iOSFacebook_shareLink( NSString * _link, NSString * _picture )
        {
            if( _link == nil )
            {
                _link = @"";
            }

            if( _picture == nil )
            {
                _picture = @"";
            }

            [[iOSFacebookPlugin sharedInstance] shareLink:_link picture:_picture];
        }
        //////////////////////////////////////////////////////////////////////////
        static void iOSFacebook_getProfilePictureLink()
        {
            [[iOSFacebookPlugin sharedInstance] getProfilePictureLink];
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    iOSFacebookScriptEmbedding::iOSFacebookScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSFacebookScriptEmbedding::~iOSFacebookScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSFacebookScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::def_function_args( _kernel, "iOSFacebookSetProvider", &Detail::iOSFacebook_setProvider );
        pybind::def_function( _kernel, "iOSFacebookLogin", &Detail::iOSFacebook_login );
        pybind::def_function( _kernel, "iOSFacebookLogout", &Detail::iOSFacebook_logout );
        pybind::def_function( _kernel, "iOSFacebookIsLoggedIn", &Detail::iOSFacebook_isLoggedIn );
        pybind::def_function( _kernel, "iOSFacebookGetAccessToken", &Detail::iOSFacebook_getAccessToken );
        pybind::def_function( _kernel, "iOSFacebookGetUserId", &Detail::iOSFacebook_getUserId );
        pybind::def_function( _kernel, "iOSFacebookGetUser", &Detail::iOSFacebook_getUser );
        pybind::def_function( _kernel, "iOSFacebookShareLink", &Detail::iOSFacebook_shareLink );
        pybind::def_function( _kernel, "iOSFacebookGetProfilePictureLink", &Detail::iOSFacebook_getProfilePictureLink );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSFacebookScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "iOSFacebookSetProvider", nullptr );
        _kernel->remove_from_module( "iOSFacebookLogin", nullptr );
        _kernel->remove_from_module( "iOSFacebookLogout", nullptr );
        _kernel->remove_from_module( "iOSFacebookIsLoggedIn", nullptr );
        _kernel->remove_from_module( "iOSFacebookGetAccessToken", nullptr );
        _kernel->remove_from_module( "iOSFacebookGetUserId", nullptr );
        _kernel->remove_from_module( "iOSFacebookGetUser", nullptr );
        _kernel->remove_from_module( "iOSFacebookShareLink", nullptr );
        _kernel->remove_from_module( "iOSFacebookGetProfilePictureLink", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}
