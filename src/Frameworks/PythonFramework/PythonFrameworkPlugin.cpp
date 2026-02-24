#include "PythonFrameworkPlugin.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/ScriptProviderServiceInterface.h"

#include "ConstsScriptEmbedding.h"
#include "EngineScriptEmbedding.h"
#include "EntityScriptEmbedding.h"
#include "GameScriptEmbedding.h"
#include "HelperScriptEmbedding.h"
#include "MathScriptEmbedding.h"
#include "KernelScriptEmbedding.h"
#include "HttpScriptEmbedding.h"
#include "EnvironmentScriptEmbedding.h"
#include "NodeScriptEmbedding.h"
#include "ResourceScriptEmbedding.h"
#include "SoundScriptEmbedding.h"
#include "AmplifierScriptEmbedding.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FrameworkFactory.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/PluginHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/ScriptEmbeddingHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( ScriptProviderService );
SERVICE_EXTERN( ScriptService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( PythonFramework, Mengine::PythonFrameworkPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonFrameworkPlugin::PythonFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonFrameworkPlugin::~PythonFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonFrameworkPlugin::registerPlugin( const PluginInterfacePtr & _plugin )
    {
        ScriptEmbeddableInterface * embeddable = _plugin->getEmbeddable();

        if( embeddable == nullptr )
        {
            return true;
        }

        const ConstString & type = embeddable->getEmbeddedType();

        ScriptEmbeddingInterfacePtr embedding = embeddable->generateEmbedding( MENGINE_DOCUMENT_FACTORABLE_PTR( embeddable ) );

        if( embedding == nullptr )
        {
            return true;
        }

        if( SCRIPT_SERVICE()
            ->addScriptEmbedding( type, embedding ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFrameworkPlugin::unregisterPlugin( const PluginInterfacePtr & _plugin )
    {
        ScriptEmbeddableInterface * embeddable = _plugin->getEmbeddable();

        if( embeddable == nullptr )
        {
            return;
        }

        const ConstString & type = embeddable->getEmbeddedType();

        SCRIPT_SERVICE()
            ->removeScriptEmbedding( type );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonFrameworkPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( ScriptProviderService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( SERVICE_CREATE( ScriptService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addScriptEmbedding<ConstsScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addScriptEmbedding<MathScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addScriptEmbedding<HelperScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addScriptEmbedding<KernelScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addScriptEmbedding<HttpScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addScriptEmbedding<HttpScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addScriptEmbedding<EnvironmentScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addScriptEmbedding<NodeScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addScriptEmbedding<ResourceScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addScriptEmbedding<EntityScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addScriptEmbedding<SoundScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addScriptEmbedding<AmplifierScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addScriptEmbedding<EngineScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_GAME, &PythonFrameworkPlugin::notifyBootstrapperInitializeGame_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_FINALIZE_GAME, &PythonFrameworkPlugin::notifyBootstrapperFinalizeGame_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_RUN, &PythonFrameworkPlugin::notifyBootstrapperRun_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFrameworkPlugin::_finalizePlugin()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_GAME );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_FINALIZE_GAME );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_RUN );

        SCRIPT_SERVICE()
            ->removeScriptEmbedding( ConstsScriptEmbedding::getFactorableType() );

        SCRIPT_SERVICE()
            ->removeScriptEmbedding( MathScriptEmbedding::getFactorableType() );

        SCRIPT_SERVICE()
            ->removeScriptEmbedding( HelperScriptEmbedding::getFactorableType() );

        SCRIPT_SERVICE()
            ->removeScriptEmbedding( KernelScriptEmbedding::getFactorableType() );

        SCRIPT_SERVICE()
            ->removeScriptEmbedding( HttpScriptEmbedding::getFactorableType() );

        SCRIPT_SERVICE()
            ->removeScriptEmbedding( EnvironmentScriptEmbedding::getFactorableType() );

        SCRIPT_SERVICE()
            ->removeScriptEmbedding( NodeScriptEmbedding::getFactorableType() );

        SCRIPT_SERVICE()
            ->removeScriptEmbedding( ResourceScriptEmbedding::getFactorableType() );

        SCRIPT_SERVICE()
            ->removeScriptEmbedding( EntityScriptEmbedding::getFactorableType() );

        SCRIPT_SERVICE()
            ->removeScriptEmbedding( SoundScriptEmbedding::getFactorableType() );

        SCRIPT_SERVICE()
            ->removeScriptEmbedding( AmplifierScriptEmbedding::getFactorableType() );

        SCRIPT_SERVICE()
            ->removeScriptEmbedding( EngineScriptEmbedding::getFactorableType() );

        SERVICE_FINALIZE( ScriptService );
        SERVICE_FINALIZE( ScriptProviderService );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFrameworkPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( ScriptService );
        SERVICE_DESTROY( ScriptProviderService );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFrameworkPlugin::notifyBootstrapperInitializeGame_()
    {
        SCRIPT_SERVICE()
            ->addScriptEmbedding( GameScriptEmbedding::getFactorableType(), Helper::makeFactorableUnique<GameScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFrameworkPlugin::notifyBootstrapperFinalizeGame_()
    {
        SCRIPT_SERVICE()
            ->removeScriptEmbedding( GameScriptEmbedding::getFactorableType() );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFrameworkPlugin::notifyBootstrapperRun_()
    {
        SCRIPTPROVIDER_SERVICE()
            ->updateScriptThread();
    }
    //////////////////////////////////////////////////////////////////////////
}
