#include "AppleFacebookScriptEmbedding.h"

#include "AppleFacebookInterface.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonDocumentTraceback.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Factory.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        class PythonAppleFacebookProvider
            : public AppleFacebookProviderInterface
        {
        public:
            PythonAppleFacebookProvider( const pybind::dict & _cbs, const pybind::args & _args )
                : m_cbs( _cbs )
                , m_args( _args )
            {
            }

        protected:
            void onFacebookLoginSuccess( const Char * token ) override
            {
                pybind::object cb = m_cbs["onFacebookLoginSuccess"];

                cb.call_args( token, m_args );
            };

            void onFacebookLoginCancel() override
            {
                pybind::object cb = m_cbs["onFacebookLoginCancel"];

                cb.call_args( m_args );
            }

            void onFacebookError( const Char * errorMessage ) override
            {
                pybind::object cb = m_cbs["onFacebookError"];

                cb.call_args( errorMessage, m_args );
            }

            void onFacebookShareSuccess( const Char * postId ) override
            {
                pybind::object cb = m_cbs["onFacebookShareSuccess"];

                cb.call_args( postId, m_args );
            }

            void onFacebookShareCancel() override
            {
                pybind::object cb = m_cbs["onFacebookShareCancel"];

                cb.call_args( m_args );
            }

            void onFacebookShareError( const Char * errorMessage ) override
            {
                pybind::object cb = m_cbs["onFacebookShareError"];

                cb.call_args( errorMessage, m_args );
            }

            void onFacebookProfilePictureLinkGet( const Char * userId, bool success, const Char * pictureURL ) override
            {
                pybind::object cb = m_cbs["onFacebookProfilePictureLinkGet"];

                cb.call_args( userId, success, pictureURL, m_args );
            }

        protected:
            pybind::dict m_cbs;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PythonAppleFacebookProvider, AppleFacebookProviderInterface> PythonAppleFacebookProviderPtr;
        //////////////////////////////////////////////////////////////////////////
        static void s_AppleFacebook_setProvider( const pybind::dict & _cbs, const pybind::args & _args )
        {
            PythonAppleFacebookProviderPtr provider = Helper::makeFactorableUnique<PythonAppleFacebookProvider>( MENGINE_DOCUMENT_PYBIND, _cbs, _args );

            APPLE_FACEBOOK_SERVICE()
                ->setProvider( provider );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_AppleFacebook_login()
        {
            APPLE_FACEBOOK_SERVICE()
                ->login();
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_AppleFacebook_logout()
        {
            APPLE_FACEBOOK_SERVICE()
                ->logout();
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_AppleFacebook_isLoggedIn()
        {
            bool result = APPLE_FACEBOOK_SERVICE()
                ->isLoggedIn();

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static const Char * s_AppleFacebook_getAccessToken()
        {
            const Char * token = APPLE_FACEBOOK_SERVICE()
                ->getAccessToken();

            return token;
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_AppleFacebook_shareLink( const Char * link, const Char * picture )
        {
            APPLE_FACEBOOK_SERVICE()
                ->shareLink( link, picture );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_AppleFacebook_getProfilePictureLink( const Char * link, const Char * picture )
        {
            APPLE_FACEBOOK_SERVICE()
                ->getProfilePictureLink();
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

        pybind::def_function_args( _kernel, "appleFacebookSetProvider", &Detail::s_AppleFacebook_setProvider );

        pybind::def_function( _kernel, "appleFacebookLogin", &Detail::s_AppleFacebook_login );
        pybind::def_function( _kernel, "appleFacebookLogout", &Detail::s_AppleFacebook_logout );
        pybind::def_function( _kernel, "appleFacebookIsLoggedIn", &Detail::s_AppleFacebook_isLoggedIn );
        pybind::def_function( _kernel, "appleFacebookGetAccessToken", &Detail::s_AppleFacebook_getAccessToken );
        pybind::def_function( _kernel, "appleFacebookShareLink", &Detail::s_AppleFacebook_shareLink );
        pybind::def_function( _kernel, "appleFacebookGetProfilePictureLink", &Detail::s_AppleFacebook_getProfilePictureLink );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFacebookScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "appleFacebookLogin", nullptr );
        _kernel->remove_from_module( "appleFacebookLogout", nullptr );
        _kernel->remove_from_module( "appleFacebookIsLoggedIn", nullptr );
        _kernel->remove_from_module( "appleFacebookGetAccessToken", nullptr );
        _kernel->remove_from_module( "appleFacebookShareLink", nullptr );
        _kernel->remove_from_module( "appleFacebookGetProfilePictureLink", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}

