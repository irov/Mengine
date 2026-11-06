#include "iOSAppleSignInScriptEmbedding.h"

#include "iOSAppleSignInInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Plugins/iOSNativePythonPlugin/iOSNativePythonTypeCast.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/Python/ApplePythonProvider.h"

#import "iOSAppleSignInPlugin.h"

@interface PythoniOSAppleSignInProvider : ApplePythonProvider<iOSAppleSignInProviderInterface>
@end

@implementation PythoniOSAppleSignInProvider

- (void)onAppleSignInSuccess:(NSDictionary<NSString *, id> *)credential {
    pybind::object py_cb = [self getMethod:@"oniOSAppleSignInSuccess"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( credential, self.m_args );
}

- (void)onAppleSignInCancel {
    pybind::object py_cb = [self getMethod:@"oniOSAppleSignInCancel"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( self.m_args );
}

- (void)onAppleSignInError:(NSInteger)code message:(NSString *)errorMessage {
    pybind::object py_cb = [self getMethod:@"oniOSAppleSignInError"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( code, errorMessage, self.m_args );
}

- (void)onAppleSignInCredentialState:(NSString *)userId state:(iOSAppleSignInCredentialState)state {
    pybind::object py_cb = [self getMethod:@"oniOSAppleSignInCredentialState"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( userId, state, self.m_args );
}

- (void)onAppleSignInCredentialStateError:(NSString *)userId code:(NSInteger)code message:(NSString *)errorMessage {
    pybind::object py_cb = [self getMethod:@"oniOSAppleSignInCredentialStateError"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( userId, code, errorMessage, self.m_args );
}

- (void)onAppleSignInCredentialRevoked:(NSString *)userId {
    pybind::object py_cb = [self getMethod:@"oniOSAppleSignInCredentialRevoked"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( userId, self.m_args );
}

@end

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void iOSAppleSignIn_setProvider( const pybind::dict & _cbs, const pybind::args & _args )
        {
            id<iOSAppleSignInProviderInterface> provider = [[PythoniOSAppleSignInProvider alloc] initWithCbs:_cbs args:_args];

            [[iOSAppleSignInPlugin sharedInstance] setProvider:provider];
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSAppleSignIn_login( BOOL _requestEmail, BOOL _requestFullName, NSString * _state, NSString * _nonce )
        {
            bool successful = [[iOSAppleSignInPlugin sharedInstance] loginWithEmail:_requestEmail
                                                                           fullName:_requestFullName
                                                                              state:_state
                                                                              nonce:_nonce];

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static NSString * iOSAppleSignIn_getStoredUserId()
        {
            return [[iOSAppleSignInPlugin sharedInstance] getStoredUserId];
        }
        //////////////////////////////////////////////////////////////////////////
        static void iOSAppleSignIn_clearStoredCredential()
        {
            [[iOSAppleSignInPlugin sharedInstance] clearStoredCredential];
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSAppleSignIn_checkCredentialState( NSString * _userId )
        {
            bool successful = [[iOSAppleSignInPlugin sharedInstance] checkCredentialState:_userId];

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSAppleSignIn_showButton( iOSAppleSignInButtonType _type, iOSAppleSignInButtonStyle _style, CGFloat _x, CGFloat _y, CGFloat _width, CGFloat _height, CGFloat _cornerRadius )
        {
            bool successful = [[iOSAppleSignInPlugin sharedInstance] showButtonWithType:_type
                                                                                  style:_style
                                                                                      x:_x
                                                                                      y:_y
                                                                                  width:_width
                                                                                 height:_height
                                                                           cornerRadius:_cornerRadius];

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static void iOSAppleSignIn_hideButton()
        {
            [[iOSAppleSignInPlugin sharedInstance] hideButton];
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSAppleSignIn_isButtonVisible()
        {
            bool visible = [[iOSAppleSignInPlugin sharedInstance] isButtonVisible];

            return visible;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    iOSAppleSignInScriptEmbedding::iOSAppleSignInScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSAppleSignInScriptEmbedding::~iOSAppleSignInScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSAppleSignInScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::enum_<iOSAppleSignInCredentialState>( _kernel, "iOSAppleSignInCredentialState" )
            .def( "IOS_APPLE_SIGNIN_CREDENTIAL_STATE_UNKNOWN", iOSAppleSignInCredentialStateUnknown )
            .def( "IOS_APPLE_SIGNIN_CREDENTIAL_STATE_REVOKED", iOSAppleSignInCredentialStateRevoked )
            .def( "IOS_APPLE_SIGNIN_CREDENTIAL_STATE_AUTHORIZED", iOSAppleSignInCredentialStateAuthorized )
            .def( "IOS_APPLE_SIGNIN_CREDENTIAL_STATE_NOT_FOUND", iOSAppleSignInCredentialStateNotFound )
            .def( "IOS_APPLE_SIGNIN_CREDENTIAL_STATE_TRANSFERRED", iOSAppleSignInCredentialStateTransferred )
            ;

        pybind::enum_<iOSAppleSignInButtonType>( _kernel, "iOSAppleSignInButtonType" )
            .def( "IOS_APPLE_SIGNIN_BUTTON_TYPE_SIGN_IN", iOSAppleSignInButtonTypeSignIn )
            .def( "IOS_APPLE_SIGNIN_BUTTON_TYPE_CONTINUE", iOSAppleSignInButtonTypeContinue )
            .def( "IOS_APPLE_SIGNIN_BUTTON_TYPE_SIGN_UP", iOSAppleSignInButtonTypeSignUp )
            ;

        pybind::enum_<iOSAppleSignInButtonStyle>( _kernel, "iOSAppleSignInButtonStyle" )
            .def( "IOS_APPLE_SIGNIN_BUTTON_STYLE_WHITE", iOSAppleSignInButtonStyleWhite )
            .def( "IOS_APPLE_SIGNIN_BUTTON_STYLE_WHITE_OUTLINE", iOSAppleSignInButtonStyleWhiteOutline )
            .def( "IOS_APPLE_SIGNIN_BUTTON_STYLE_BLACK", iOSAppleSignInButtonStyleBlack )
            ;

        pybind::def_function_args( _kernel, "iOSAppleSignInSetProvider", &Detail::iOSAppleSignIn_setProvider );
        pybind::def_function( _kernel, "iOSAppleSignInLogin", &Detail::iOSAppleSignIn_login );
        pybind::def_function( _kernel, "iOSAppleSignInGetStoredUserId", &Detail::iOSAppleSignIn_getStoredUserId );
        pybind::def_function( _kernel, "iOSAppleSignInClearStoredCredential", &Detail::iOSAppleSignIn_clearStoredCredential );
        pybind::def_function( _kernel, "iOSAppleSignInCheckCredentialState", &Detail::iOSAppleSignIn_checkCredentialState );
        pybind::def_function( _kernel, "iOSAppleSignInShowButton", &Detail::iOSAppleSignIn_showButton );
        pybind::def_function( _kernel, "iOSAppleSignInHideButton", &Detail::iOSAppleSignIn_hideButton );
        pybind::def_function( _kernel, "iOSAppleSignInIsButtonVisible", &Detail::iOSAppleSignIn_isButtonVisible );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSAppleSignInScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "IOS_APPLE_SIGNIN_CREDENTIAL_STATE_UNKNOWN", nullptr );
        _kernel->remove_from_module( "IOS_APPLE_SIGNIN_CREDENTIAL_STATE_REVOKED", nullptr );
        _kernel->remove_from_module( "IOS_APPLE_SIGNIN_CREDENTIAL_STATE_AUTHORIZED", nullptr );
        _kernel->remove_from_module( "IOS_APPLE_SIGNIN_CREDENTIAL_STATE_NOT_FOUND", nullptr );
        _kernel->remove_from_module( "IOS_APPLE_SIGNIN_CREDENTIAL_STATE_TRANSFERRED", nullptr );

        _kernel->remove_from_module( "IOS_APPLE_SIGNIN_BUTTON_TYPE_SIGN_IN", nullptr );
        _kernel->remove_from_module( "IOS_APPLE_SIGNIN_BUTTON_TYPE_CONTINUE", nullptr );
        _kernel->remove_from_module( "IOS_APPLE_SIGNIN_BUTTON_TYPE_SIGN_UP", nullptr );

        _kernel->remove_from_module( "IOS_APPLE_SIGNIN_BUTTON_STYLE_WHITE", nullptr );
        _kernel->remove_from_module( "IOS_APPLE_SIGNIN_BUTTON_STYLE_WHITE_OUTLINE", nullptr );
        _kernel->remove_from_module( "IOS_APPLE_SIGNIN_BUTTON_STYLE_BLACK", nullptr );

        _kernel->remove_from_module( "iOSAppleSignInSetProvider", nullptr );
        _kernel->remove_from_module( "iOSAppleSignInLogin", nullptr );
        _kernel->remove_from_module( "iOSAppleSignInGetStoredUserId", nullptr );
        _kernel->remove_from_module( "iOSAppleSignInClearStoredCredential", nullptr );
        _kernel->remove_from_module( "iOSAppleSignInCheckCredentialState", nullptr );
        _kernel->remove_from_module( "iOSAppleSignInShowButton", nullptr );
        _kernel->remove_from_module( "iOSAppleSignInHideButton", nullptr );
        _kernel->remove_from_module( "iOSAppleSignInIsButtonVisible", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}
