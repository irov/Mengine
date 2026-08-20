#include "iOSAppleSignInScriptEmbedding.h"

#include "iOSAppleSignInInterface.h"

#include "Environment/Python/PythonIncluder.h"

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

    py_cb.call_args( (NSDictionary *)credential, self.m_args );
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

    py_cb.call_args( userId, (NSInteger)state, self.m_args );
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
        static bool iOSAppleSignIn_isAvailable()
        {
            return [[iOSAppleSignInPlugin sharedInstance] isAvailable] == YES;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSAppleSignIn_login( bool _requestEmail, bool _requestFullName, const Char * _state, const Char * _nonce )
        {
            NSString * state = _state != nullptr ? [NSString stringWithUTF8String:_state] : nil;
            NSString * nonce = _nonce != nullptr ? [NSString stringWithUTF8String:_nonce] : nil;

            return [[iOSAppleSignInPlugin sharedInstance] loginWithEmail:_requestEmail == true ? YES : NO
                                                               fullName:_requestFullName == true ? YES : NO
                                                                  state:state
                                                                  nonce:nonce] == YES;
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
        static bool iOSAppleSignIn_checkCredentialState( const Char * _userId )
        {
            NSString * userId = _userId != nullptr ? [NSString stringWithUTF8String:_userId] : nil;

            return [[iOSAppleSignInPlugin sharedInstance] checkCredentialState:userId] == YES;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSAppleSignIn_showButton( int32_t _type, int32_t _style, float _x, float _y, float _width, float _height, float _cornerRadius )
        {
            return [[iOSAppleSignInPlugin sharedInstance] showButtonWithType:(iOSAppleSignInButtonType)_type
                                                                       style:(iOSAppleSignInButtonStyle)_style
                                                                           x:(CGFloat)_x
                                                                           y:(CGFloat)_y
                                                                       width:(CGFloat)_width
                                                                      height:(CGFloat)_height
                                                                cornerRadius:(CGFloat)_cornerRadius] == YES;
        }
        //////////////////////////////////////////////////////////////////////////
        static void iOSAppleSignIn_hideButton()
        {
            [[iOSAppleSignInPlugin sharedInstance] hideButton];
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSAppleSignIn_isButtonVisible()
        {
            return [[iOSAppleSignInPlugin sharedInstance] isButtonVisible] == YES;
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
        pybind::def_const<int32_t>( _kernel, "IOS_APPLE_SIGNIN_CREDENTIAL_STATE_UNKNOWN", iOSAppleSignInCredentialStateUnknown );
        pybind::def_const<int32_t>( _kernel, "IOS_APPLE_SIGNIN_CREDENTIAL_STATE_REVOKED", iOSAppleSignInCredentialStateRevoked );
        pybind::def_const<int32_t>( _kernel, "IOS_APPLE_SIGNIN_CREDENTIAL_STATE_AUTHORIZED", iOSAppleSignInCredentialStateAuthorized );
        pybind::def_const<int32_t>( _kernel, "IOS_APPLE_SIGNIN_CREDENTIAL_STATE_NOT_FOUND", iOSAppleSignInCredentialStateNotFound );
        pybind::def_const<int32_t>( _kernel, "IOS_APPLE_SIGNIN_CREDENTIAL_STATE_TRANSFERRED", iOSAppleSignInCredentialStateTransferred );

        pybind::def_const<int32_t>( _kernel, "IOS_APPLE_SIGNIN_BUTTON_TYPE_SIGN_IN", iOSAppleSignInButtonTypeSignIn );
        pybind::def_const<int32_t>( _kernel, "IOS_APPLE_SIGNIN_BUTTON_TYPE_CONTINUE", iOSAppleSignInButtonTypeContinue );
        pybind::def_const<int32_t>( _kernel, "IOS_APPLE_SIGNIN_BUTTON_TYPE_SIGN_UP", iOSAppleSignInButtonTypeSignUp );

        pybind::def_const<int32_t>( _kernel, "IOS_APPLE_SIGNIN_BUTTON_STYLE_WHITE", iOSAppleSignInButtonStyleWhite );
        pybind::def_const<int32_t>( _kernel, "IOS_APPLE_SIGNIN_BUTTON_STYLE_WHITE_OUTLINE", iOSAppleSignInButtonStyleWhiteOutline );
        pybind::def_const<int32_t>( _kernel, "IOS_APPLE_SIGNIN_BUTTON_STYLE_BLACK", iOSAppleSignInButtonStyleBlack );

        pybind::def_function_args( _kernel, "iOSAppleSignInSetProvider", &Detail::iOSAppleSignIn_setProvider );
        pybind::def_function( _kernel, "iOSAppleSignInIsAvailable", &Detail::iOSAppleSignIn_isAvailable );
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
        _kernel->remove_from_module( "iOSAppleSignInIsAvailable", nullptr );
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
