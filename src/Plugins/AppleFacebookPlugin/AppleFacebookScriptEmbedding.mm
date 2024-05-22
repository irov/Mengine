#include "AppleFacebookScriptEmbedding.h"

#include "AppleFacebookInterface.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocumentTraceback.h"
#include "Environment/Python/PythonCallbackProvider.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        class PythonAppleFacebookProvider
            : public PythonCallbackProvider<AppleFacebookProviderInterface>
        {
        public:
            PythonAppleFacebookProvider( const pybind::dict & _cbs, const pybind::args & _args )
                : PythonCallbackProvider<AppleFacebookProviderInterface>( _cbs, _args )
            {
            }

        protected:
            void onFacebookLoginSuccess( const Params & _params ) override
            {
                this->call_cbs( "onAppleFacebookLoginSuccess", _params );
            };

            void onFacebookLoginCancel() override
            {
                this->call_cbs( "onAppleFacebookLoginCancel" );
            }

            void onFacebookError( int32_t _code, const Char * _errorMessage ) override
            {
                this->call_cbs( "onAppleFacebookError", _code, _errorMessage );
            }

            void onFacebookShareSuccess( const Char * _postId ) override
            {
                this->call_cbs( "onAppleFacebookShareSuccess", _postId );
            }

            void onFacebookShareCancel() override
            {
                this->call_cbs( "onAppleFacebookShareCancel" );
            }

            void onFacebookShareError( int32_t _code, const Char * _errorMessage ) override
            {
                this->call_cbs( "onAppleFacebookShareError", _code, _errorMessage );
            }

            void onFacebookProfilePictureLinkGetSuccess( const Char * _userId, bool _success, const Char * _pictureURL ) override
            {
                this->call_cbs( "onAppleFacebookProfilePictureLinkGetSuccess", _userId, _success, _pictureURL );
            }
            
            void onFacebookProfilePictureLinkGetError( int32_t _code, const Char * _errorMessage ) override
            {
                this->call_cbs( "onAppleFacebookProfilePictureLinkGetError", _code, _errorMessage );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        static void s_AppleFacebook_setProvider( const pybind::dict & _cbs, const pybind::args & _args )
        {
            AppleFacebookProviderInterfacePtr provider = Helper::makeFactorableUnique<PythonAppleFacebookProvider>( MENGINE_DOCUMENT_PYBIND, _cbs, _args );

            APPLE_FACEBOOK_SERVICE()
                ->setProvider( provider );
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_AppleFacebook_getAccessToken( pybind::kernel_interface * _kernel )
        {
            Char token[256] = {'\0'};
            if( APPLE_FACEBOOK_SERVICE()
               ->getAccessToken( token, 256 ) == false )
            {
                return _kernel->ret_none();
            }
            
            PyObject * token_py = pybind::ptr( _kernel, token );

            return token_py;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_AppleFacebook_getUserId( pybind::kernel_interface * _kernel )
        {
            Char userId[256] = {'\0'};
            if( APPLE_FACEBOOK_SERVICE()
               ->getUserId( userId, 256 ) == false )
            {
                return _kernel->ret_none();
            }
            
            PyObject * userId_py = pybind::ptr( _kernel, userId );

            return userId_py;
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
        SCRIPT_SERVICE()
            ->setAvailablePlugin( "AppleFacebook", true );

        AppleFacebookServiceInterface * service = APPLE_FACEBOOK_SERVICE();

        pybind::def_function_args( _kernel, "appleFacebookSetProvider", &Detail::s_AppleFacebook_setProvider );

        pybind::def_functor( _kernel, "appleFacebookLogin", service, &AppleFacebookServiceInterface::login );
        pybind::def_functor( _kernel, "appleFacebookLogout", service, &AppleFacebookServiceInterface::logout );
        pybind::def_functor( _kernel, "appleFacebookIsAccessSuccess", service, &AppleFacebookServiceInterface::isAccessSuccess );
        pybind::def_functor( _kernel, "appleFacebookIsAuthenticationSuccess", service, &AppleFacebookServiceInterface::isAuthenticationSuccess );
        pybind::def_function_kernel( _kernel, "appleFacebookGetAccessToken", &Detail::s_AppleFacebook_getAccessToken );
        pybind::def_function_kernel( _kernel, "appleFacebookGetUserId", &Detail::s_AppleFacebook_getUserId );
        pybind::def_functor( _kernel, "appleFacebookShareLink", service, &AppleFacebookServiceInterface::shareLink );
        pybind::def_functor( _kernel, "appleFacebookGetProfilePictureLink", service, &AppleFacebookServiceInterface::getProfilePictureLink );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFacebookScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "appleFacebookLogin", nullptr );
        _kernel->remove_from_module( "appleFacebookLogout", nullptr );
        _kernel->remove_from_module( "appleFacebookIsAccessSuccess", nullptr );
        _kernel->remove_from_module( "appleFacebookIsAuthenticationSuccess", nullptr );
        _kernel->remove_from_module( "appleFacebookGetAccessToken", nullptr );
        _kernel->remove_from_module( "appleFacebookGetUserId", nullptr );
        _kernel->remove_from_module( "appleFacebookShareLink", nullptr );
        _kernel->remove_from_module( "appleFacebookGetProfilePictureLink", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}

