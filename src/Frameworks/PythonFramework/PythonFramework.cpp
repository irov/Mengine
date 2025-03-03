#include "PythonFramework.h"

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

#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonFramework::PythonFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonFramework::~PythonFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonFramework::_initializeFramework()
    {
        if( SCRIPT_SERVICE()
            ->addScriptEmbedding( ConstsScriptEmbedding::getFactorableType(), Helper::makeFactorableUnique<ConstsScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( SCRIPT_SERVICE()
            ->addScriptEmbedding( MathScriptEmbedding::getFactorableType(), Helper::makeFactorableUnique<MathScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( SCRIPT_SERVICE()
            ->addScriptEmbedding( HelperScriptEmbedding::getFactorableType(), Helper::makeFactorableUnique<HelperScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( SCRIPT_SERVICE()
            ->addScriptEmbedding( KernelScriptEmbedding::getFactorableType(), Helper::makeFactorableUnique<KernelScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( SCRIPT_SERVICE()
            ->addScriptEmbedding( HttpScriptEmbedding::getFactorableType(), Helper::makeFactorableUnique<HttpScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( SCRIPT_SERVICE()
            ->addScriptEmbedding( EnvironmentScriptEmbedding::getFactorableType(), Helper::makeFactorableUnique<EnvironmentScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( SCRIPT_SERVICE()
            ->addScriptEmbedding( NodeScriptEmbedding::getFactorableType(), Helper::makeFactorableUnique<NodeScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( SCRIPT_SERVICE()
            ->addScriptEmbedding( ResourceScriptEmbedding::getFactorableType(), Helper::makeFactorableUnique<ResourceScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( SCRIPT_SERVICE()
            ->addScriptEmbedding( EntityScriptEmbedding::getFactorableType(), Helper::makeFactorableUnique<EntityScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( SCRIPT_SERVICE()
            ->addScriptEmbedding( SoundScriptEmbedding::getFactorableType(), Helper::makeFactorableUnique<SoundScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }
        
        if( SCRIPT_SERVICE()
            ->addScriptEmbedding( AmplifierScriptEmbedding::getFactorableType(), Helper::makeFactorableUnique<AmplifierScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( SCRIPT_SERVICE()
            ->addScriptEmbedding( EngineScriptEmbedding::getFactorableType(), Helper::makeFactorableUnique<EngineScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_NOTIFY( NOTIFICATOR_SCRIPT_EMBEDDING_END );

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_GAME, &PythonFramework::notifyBootstrapperInitializeGame, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_FINALIZE_GAME, &PythonFramework::notifyBootstrapperFinalizeGame, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_ENGINE_STOP_SERVICES, &PythonFramework::notifyEngineStopServices, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFramework::_finalizeFramework()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_GAME );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_FINALIZE_GAME );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_ENGINE_STOP_SERVICES );

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
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonFramework::_runFramework()
    {
        SCRIPTPROVIDER_SERVICE()
            ->updateScriptThread();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFramework::_stopFramework()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFramework::notifyBootstrapperInitializeGame()
    {
        SCRIPT_SERVICE()
            ->addScriptEmbedding( GameScriptEmbedding::getFactorableType(), Helper::makeFactorableUnique<GameScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFramework::notifyBootstrapperFinalizeGame()
    {
        SCRIPT_SERVICE()
            ->removeScriptEmbedding( GameScriptEmbedding::getFactorableType() );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFramework::notifyEngineStopServices()
    {
        NOTIFICATION_NOTIFY( NOTIFICATOR_SCRIPT_EJECTING );
    }
    //////////////////////////////////////////////////////////////////////////
}
