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
    namespace
    {
        //////////////////////////////////////////////////////////////////////////
        class PythonAppleGameCenterProvider
            : public AppleGameCenterProviderInterface
            , public Factorable
        {
        public:
            PythonAppleGameCenterProvider( const pybind::object & _cb, const pybind::args & _args )
                : m_cb(_cb)
                , m_args(_args)
            {
            }
            
        protected:
            void onAppleGameConterAuthenticate( bool _successful ) override
            {
                m_cb.call_args( _successful, m_args );
            }
        
        protected:
            pybind::object m_cb;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        class AppleGameCenterScriptMethod
            : public Factorable
        {
        public:
            AppleGameCenterScriptMethod()
            {
            }

            ~AppleGameCenterScriptMethod() override
            {
            }

        public:
            bool initialize()
            {
                return true;
            }

            void finalize()
            {
            }
            
        public:
            void setProvider( const pybind::object & _cb, const pybind::args & _args )
            {
                AppleGameCenterProviderInterfacePtr provider = Helper::makeFactorableUnique<PythonAppleGameCenterProvider>( MENGINE_DOCUMENT_PYBIND, _cb, _args );
                
                APPLE_GAMECENTER_SERVICE()
                    ->setProvider( provider );
            }
            
            void removeProvider()
            {
                APPLE_GAMECENTER_SERVICE()
                    ->setProvider( nullptr );
            }

        public:
            bool connect()
            {
                bool result = APPLE_GAMECENTER_SERVICE()
                    ->connect();

                return result;
            }
            
            bool isConnect()
            {
                bool result = APPLE_GAMECENTER_SERVICE()
                    ->isConnect();

                return result;
            }
            
        public:
            bool reportAchievement( const ConstString & _achievementName, double _percentComplete, const pybind::object & _cb, const pybind::args & _args )
            {
                bool result = APPLE_GAMECENTER_SERVICE()
                    ->reportAchievement( _achievementName, _percentComplete, [_cb, _args](bool _successful, double _percentComplete) {
                        _cb.call_args( _successful, _percentComplete, _args );
                } );
                
                return result;
            }

            bool checkAchievement( const ConstString & _achievementName ) const
            {
                bool result = APPLE_GAMECENTER_SERVICE()
                    ->checkAchievement( _achievementName );

                return result;
            }

        public:
            bool reportScore( const ConstString & _key, int64_t _score, const pybind::object & _cb, const pybind::args & _args )
            {
                bool result = APPLE_GAMECENTER_SERVICE()
                    ->reportScore( _key, _score, [_cb, _args](bool _successful) {
                    _cb.call_args( _successful, _args );
                } );
                
                return result;
            }

        public:

        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<AppleGameCenterScriptMethod> AppleGameCenterScriptMethodPtr;
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
        AppleGameCenterScriptMethodPtr scriptMethod = Helper::makeFactorableUnique<AppleGameCenterScriptMethod>( MENGINE_DOCUMENT_FACTORABLE );

        if( scriptMethod->initialize() == false )
        {
            return false;
        }

        SCRIPT_SERVICE()
            ->setAvailablePlugin( "AppleGameCenter", true );

        pybind::def_functor_args( _kernel, "appleGameCenterSetProvider", scriptMethod, &AppleGameCenterScriptMethod::setProvider );
        pybind::def_functor( _kernel, "appleGameCenterRemoveProvider", scriptMethod, &AppleGameCenterScriptMethod::removeProvider );
        pybind::def_functor( _kernel, "appleGameCenterConnect", scriptMethod, &AppleGameCenterScriptMethod::connect );
        pybind::def_functor( _kernel, "appleGameCenterIsConnect", scriptMethod, &AppleGameCenterScriptMethod::isConnect );
        pybind::def_functor_args( _kernel, "appleGameCenterReportAchievement", scriptMethod, &AppleGameCenterScriptMethod::reportAchievement );
        pybind::def_functor( _kernel, "appleGameCenterCheckAchievement", scriptMethod, &AppleGameCenterScriptMethod::checkAchievement );
        pybind::def_functor_args( _kernel, "appleGameCenterReportScore", scriptMethod, &AppleGameCenterScriptMethod::reportScore );

        m_implement = scriptMethod;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleGameCenterScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        MENGINE_UNUSED( _kernel );

        AppleGameCenterScriptMethodPtr scriptMethod = m_implement;
        scriptMethod->finalize();

        m_implement = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}

