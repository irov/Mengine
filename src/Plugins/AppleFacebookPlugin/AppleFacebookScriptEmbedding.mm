#include "AppleFacebookScriptEmbedding.h"

#include "AppleFacebookInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocument.h"

#import "Environment/Python/ApplePythonProvider.h"
#import "Environment/Apple/AppleDetail.h"

#import "AppleFacebookPlugin.h"

#include "Kernel/VectorConstString.h"

@interface PythonAppleFacebookProvider : ApplePythonProvider<AppleFacebookProviderInterface>
@end

@implementation PythonAppleFacebookProvider

- (void)onFacebookLoginSuccess:(NSDictionary<NSString *,NSString *> *)params {
    pybind::object py_cb = [self getMethod:@"onAppleFacebookLoginSuccess"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( params, self.m_args );
}

- (void)onFacebookLoginCancel {
    pybind::object py_cb = [self getMethod:@"onAppleFacebookLoginCancel"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( self.m_args );
}

- (void)onFacebookError:(NSInteger)code message:(NSString *)errorMessage {
    pybind::object py_cb = [self getMethod:@"onAppleFacebookError"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( code, errorMessage, self.m_args );
}

- (void)onFacebookShareSuccess:(NSString *)postId {
    pybind::object py_cb = [self getMethod:@"onAppleFacebookShareSuccess"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( postId, self.m_args );
}

- (void)onFacebookShareCancel {
    pybind::object py_cb = [self getMethod:@"onAppleFacebookShareCancel"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( self.m_args );
}

- (void)onFacebookShareError:(NSInteger)code message:(NSString *)errorMessage {
    pybind::object py_cb = [self getMethod:@"onAppleFacebookShareError"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( code, errorMessage, self.m_args );
}

- (void)onFacebookProfilePictureLinkGetSuccess:(NSString *)userId pictureURL:(NSString *)pictureURL {
    pybind::object py_cb = [self getMethod:@"onAppleFacebookProfilePictureLinkGetSuccess"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( userId, pictureURL, self.m_args );
}

- (void)onFacebookProfilePictureLinkGetError:(NSInteger)code message:(NSString *)errorMessage {
    pybind::object py_cb = [self getMethod:@"onAppleFacebookProfilePictureLinkGetError"];

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
        static void AppleFacebook_setProvider( const pybind::dict & _cbs, const pybind::args & _args )
        {
            id<AppleFacebookProviderInterface> provider = [[PythonAppleFacebookProvider alloc] initWithCbs:_cbs args:_args];

            [[AppleFacebookPlugin sharedInstance] setProvider:provider];
        }
        //////////////////////////////////////////////////////////////////////////
        static bool AppleFacebook_login( const VectorConstString & _permissions )
        {
            NSArray<NSString *> * permissions = [AppleDetail getNSArrayFromVectorConstString:_permissions];

            return [[AppleFacebookPlugin sharedInstance] login:permissions];
        }
        //////////////////////////////////////////////////////////////////////////
        static void AppleFacebook_logout()
        {
            [[AppleFacebookPlugin sharedInstance] logout];
        }
        //////////////////////////////////////////////////////////////////////////
        static bool AppleFacebook_isLoggedIn()
        {
            return [[AppleFacebookPlugin sharedInstance] isLoggedIn];
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * AppleFacebook_getAccessToken( pybind::kernel_interface * _kernel )
        {
            NSString * accessToken = [[AppleFacebookPlugin sharedInstance] getAccessToken];

            if( accessToken == nil )
            {
                return _kernel->ret_none();
            }

            return pybind::ptr( _kernel, accessToken.UTF8String );
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * AppleFacebook_getUserId( pybind::kernel_interface * _kernel )
        {
            NSString * userId = [[AppleFacebookPlugin sharedInstance] getUserId];

            if( userId == nil )
            {
                return _kernel->ret_none();
            }

            return pybind::ptr( _kernel, userId.UTF8String );
        }
        //////////////////////////////////////////////////////////////////////////
        static void AppleFacebook_shareLink( const Char * _link, const Char * _picture )
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

            [[AppleFacebookPlugin sharedInstance] shareLink:link picture:picture];
        }
        //////////////////////////////////////////////////////////////////////////
        static void AppleFacebook_getProfilePictureLink()
        {
            [[AppleFacebookPlugin sharedInstance] getProfilePictureLink];
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    AppleFacebookScriptEmbedding::AppleFacebookScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleFacebookScriptEmbedding::~AppleFacebookScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFacebookScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::def_function_args( _kernel, "appleFacebookSetProvider", &Detail::AppleFacebook_setProvider );
        pybind::def_function( _kernel, "appleFacebookLogin", &Detail::AppleFacebook_login );
        pybind::def_function( _kernel, "appleFacebookLogout", &Detail::AppleFacebook_logout );
        pybind::def_function( _kernel, "appleFacebookIsLoggedIn", &Detail::AppleFacebook_isLoggedIn );
        pybind::def_function_kernel( _kernel, "appleFacebookGetAccessToken", &Detail::AppleFacebook_getAccessToken );
        pybind::def_function_kernel( _kernel, "appleFacebookGetUserId", &Detail::AppleFacebook_getUserId );
        pybind::def_function( _kernel, "appleFacebookShareLink", &Detail::AppleFacebook_shareLink );
        pybind::def_function( _kernel, "appleFacebookGetProfilePictureLink", &Detail::AppleFacebook_getProfilePictureLink );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFacebookScriptEmbedding::eject( pybind::kernel_interface * _kernel )
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

