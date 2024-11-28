#include "AppleGameCenterScriptEmbedding.h"

#include "AppleGameCenterInterface.h"

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
        class PythonAppleGameCenterProvider
            : public PythonCallbackProvider<AppleGameCenterProviderInterface>
        {
        public:
            PythonAppleGameCenterProvider( const pybind::dict & _cbs, const pybind::args & _args )
                : PythonCallbackProvider<AppleGameCenterProviderInterface>( _cbs, _args )
            {
            }

        protected:
            void onAppleGameCenterAuthenticate( bool _successful ) override
            {
                this->call_cbs( "onAppleGameCenterAuthenticate", _successful );
            }

            void onAppleGameCenterSynchronizate( bool _successful ) override
            {
                this->call_cbs( "onAppleGameCenterSynchronizate", _successful );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        static void AppleGameCenter_setProvider(const pybind::dict & _cbs, const pybind::args & _args )
        {
            AppleGameCenterProviderInterfacePtr provider = Helper::makeFactorableUnique<PythonAppleGameCenterProvider>( MENGINE_DOCUMENT_PYBIND, _cbs, _args );

            APPLE_GAMECENTER_SERVICE()
                ->setProvider( provider );
        }
        //////////////////////////////////////////////////////////////////////////
        static void AppleGameCenter_removeProvider()
        {
            APPLE_GAMECENTER_SERVICE()
                ->setProvider( nullptr );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool AppleGameCenter_reportAchievement(const ConstString & _achievementName, double _percentComplete, const pybind::object & _cb, const pybind::args & _args )
        {
            bool result = APPLE_GAMECENTER_SERVICE()
                ->reportAchievement( _achievementName, _percentComplete, [_cb, _args]( bool _successful )
            {
                _cb.call_args( _successful, _args );
            } );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool AppleGameCenter_reportScore( const ConstString & _key, int64_t _score, const pybind::object & _cb, const pybind::args & _args )
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
            ->setAvailablePlugin( STRINGIZE_STRING_LOCAL("AppleGameCenter"), true );
        
        AppleGameCenterServiceInterface * service = APPLE_GAMECENTER_SERVICE();

        pybind::def_function_args( _kernel, "appleGameCenterSetProvider", &Detail::AppleGameCenter_setProvider );
        pybind::def_function( _kernel, "appleGameCenterRemoveProvider", &Detail::AppleGameCenter_removeProvider );
        pybind::def_functor( _kernel, "appleGameCenterConnect", service, &AppleGameCenterServiceInterface::connect );
        pybind::def_functor( _kernel, "appleGameCenterIsConnect", service, &AppleGameCenterServiceInterface::isConnect );
        pybind::def_function_args( _kernel, "appleGameCenterReportAchievement", &Detail::AppleGameCenter_reportAchievement );
        pybind::def_functor( _kernel, "appleGameCenterCheckAchievement", service, &AppleGameCenterServiceInterface::checkAchievement );
        pybind::def_functor( _kernel, "appleGameCenterResetAchievements", service, &AppleGameCenterServiceInterface::resetAchievements );
        pybind::def_function_args( _kernel, "appleGameCenterReportScore", &Detail::AppleGameCenter_reportScore );

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

