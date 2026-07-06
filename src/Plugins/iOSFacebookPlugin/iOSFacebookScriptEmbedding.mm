#include "iOSFacebookScriptEmbedding.h"

#include "iOSFacebookInterface.h"

#include "Environment/Python/PythonIncluder.h"
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
            return [[iOSFacebookPlugin sharedInstance] isLoggedIn];
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * iOSFacebook_getAccessToken( pybind::kernel_interface * _kernel )
        {
            NSString * accessToken = [[iOSFacebookPlugin sharedInstance] getAccessToken];

            if( accessToken == nil )
            {
                return _kernel->ret_none();
            }

            return pybind::ptr( _kernel, accessToken.UTF8String );
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * iOSFacebook_getUserId( pybind::kernel_interface * _kernel )
        {
            NSString * userId = [[iOSFacebookPlugin sharedInstance] getUserId];

            if( userId == nil )
            {
                return _kernel->ret_none();
            }

            return pybind::ptr( _kernel, userId.UTF8String );
        }
        //////////////////////////////////////////////////////////////////////////
        static void iOSFacebook_shareLink( const Char * _link, const Char * _picture )
        {
            NSString * link = _link != nullptr ? [NSString stringWithUTF8String:_link] : @"";
            NSString * picture = _picture != nullptr ? [NSString stringWithUTF8String:_picture] : @"";

            if( link == nil )
            {
                link = @"";
            }

            if( picture == nil )
            {
                picture = @"";
            }

            [[iOSFacebookPlugin sharedInstance] shareLink:link picture:picture];
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
        pybind::def_function_args( _kernel, "appleFacebookSetProvider", &Detail::iOSFacebook_setProvider );
        pybind::def_function( _kernel, "appleFacebookLogin", &Detail::iOSFacebook_login );
        pybind::def_function( _kernel, "appleFacebookLogout", &Detail::iOSFacebook_logout );
        pybind::def_function( _kernel, "appleFacebookIsLoggedIn", &Detail::iOSFacebook_isLoggedIn );
        pybind::def_function_kernel( _kernel, "appleFacebookGetAccessToken", &Detail::iOSFacebook_getAccessToken );
        pybind::def_function_kernel( _kernel, "appleFacebookGetUserId", &Detail::iOSFacebook_getUserId );
        pybind::def_function( _kernel, "appleFacebookShareLink", &Detail::iOSFacebook_shareLink );
        pybind::def_function( _kernel, "appleFacebookGetProfilePictureLink", &Detail::iOSFacebook_getProfilePictureLink );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSFacebookScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "appleFacebookSetProvider", nullptr );
        _kernel->remove_from_module( "appleFacebookLogin", nullptr );
        _kernel->remove_from_module( "appleFacebookLogout", nullptr );
        _kernel->remove_from_module( "appleFacebookIsLoggedIn", nullptr );
        _kernel->remove_from_module( "appleFacebookGetAccessToken", nullptr );
        _kernel->remove_from_module( "appleFacebookGetUserId", nullptr );
        _kernel->remove_from_module( "appleFacebookShareLink", nullptr );
        _kernel->remove_from_module( "appleFacebookGetProfilePictureLink", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}
