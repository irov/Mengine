#include "AppleGameCenterScriptEmbedding.h"

#include "AppleGameCenterInterface.h"

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
        class PythonAppleGameCenterProvider
            : public AppleGameCenterProviderInterface
            , public Factorable
        {
        public:
            PythonAppleGameCenterProvider( const pybind::dict & _cbs, const pybind::args & _args )
                : m_cbs( _cbs )
                , m_args( _args )
            {
            }

        protected:
            void onAppleGameCenterAuthenticate( bool _successful ) override
            {
                pybind::object cb = m_cbs["onAppleGameCenterAuthenticate"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( _successful, m_args );
            }

            void onAppleGameCenterSynchronizate( bool _successful ) override
            {
                pybind::object cb = m_cbs["onAppleGameCenterSynchronizate"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( _successful, m_args );
            }

        protected:
            pybind::dict m_cbs;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        static void s_AppleGameCenter_setProvider( const pybind::dict & _cbs, const pybind::args & _args )
        {
            AppleGameCenterProviderInterfacePtr provider = Helper::makeFactorableUnique<PythonAppleGameCenterProvider>( MENGINE_DOCUMENT_PYBIND, _cbs, _args );

            APPLE_GAMECENTER_SERVICE()
                ->setProvider( provider );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_AppleGameCenter_removeProvider()
        {
            APPLE_GAMECENTER_SERVICE()
                ->setProvider( nullptr );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_AppleGameCenter_connect()
        {
            bool result = APPLE_GAMECENTER_SERVICE()
                ->connect();

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_AppleGameCenter_isConnect()
        {
            bool result = APPLE_GAMECENTER_SERVICE()
                ->isConnect();

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_AppleGameCenter_reportAchievement( const ConstString & _achievementName, double _percentComplete, const pybind::object & _cb, const pybind::args & _args )
        {
            bool result = APPLE_GAMECENTER_SERVICE()
                ->reportAchievement( _achievementName, _percentComplete, [_cb, _args]( bool _successful )
            {
                _cb.call_args( _successful, _args );
            } );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_AppleGameCenter_checkAchievement( const ConstString & _achievementName )
        {
            bool result = APPLE_GAMECENTER_SERVICE()
                ->checkAchievement( _achievementName );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_AppleGameCenter_resetAchievements()
        {
            bool result = APPLE_GAMECENTER_SERVICE()
                ->resetAchievements();

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_AppleGameCenter_reportScore( const ConstString & _key, int64_t _score, const pybind::object & _cb, const pybind::args & _args )
        {
            bool result = APPLE_GAMECENTER_SERVICE()
                ->reportScore( _key, _score, [_cb, _args]( bool _successful )
            {
                _cb.call_args( _successful, _args );
            } );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    AppleGameCenterScriptEmbedding::AppleGameCenterScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleGameCenterScriptEmbedding::~AppleGameCenterScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleGameCenterScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        SCRIPT_SERVICE()
            ->setAvailablePlugin( "AppleGameCenter", true );

        pybind::def_function_args( _kernel, "appleGameCenterSetProvider", &Detail::s_AppleGameCenter_setProvider );
        pybind::def_function( _kernel, "appleGameCenterRemoveProvider", &Detail::s_AppleGameCenter_removeProvider );
        pybind::def_function( _kernel, "appleGameCenterConnect", &Detail::s_AppleGameCenter_connect );
        pybind::def_function( _kernel, "appleGameCenterIsConnect", &Detail::s_AppleGameCenter_isConnect );
        pybind::def_function_args( _kernel, "appleGameCenterReportAchievement", &Detail::s_AppleGameCenter_reportAchievement );
        pybind::def_function( _kernel, "appleGameCenterCheckAchievement", &Detail::s_AppleGameCenter_checkAchievement );
        pybind::def_function( _kernel, "appleGameCenterResetAchievements", &Detail::s_AppleGameCenter_resetAchievements );
        pybind::def_function_args( _kernel, "appleGameCenterReportScore", &Detail::s_AppleGameCenter_reportScore );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleGameCenterScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "appleGameCenterSetProvider", nullptr );
        _kernel->remove_from_module( "appleGameCenterRemoveProvider", nullptr );
        _kernel->remove_from_module( "appleGameCenterConnect", nullptr );
        _kernel->remove_from_module( "appleGameCenterIsConnect", nullptr );
        _kernel->remove_from_module( "appleGameCenterReportAchievement", nullptr );
        _kernel->remove_from_module( "appleGameCenterCheckAchievement", nullptr );
        _kernel->remove_from_module( "appleGameCenterResetAchievements", nullptr );
        _kernel->remove_from_module( "appleGameCenterReportScore", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}

