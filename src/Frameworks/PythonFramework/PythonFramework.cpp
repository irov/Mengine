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
            ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "ConstsScriptEmbedding" ), Helper::makeFactorableUnique<ConstsScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( SCRIPT_SERVICE()
            ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "MathScriptEmbedding" ), Helper::makeFactorableUnique<MathScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( SCRIPT_SERVICE()
            ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "HelperScriptEmbedding" ), Helper::makeFactorableUnique<HelperScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( SCRIPT_SERVICE()
            ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "KernelScriptEmbedding" ), Helper::makeFactorableUnique<KernelScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( SCRIPT_SERVICE()
            ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "HttpScriptEmbedding" ), Helper::makeFactorableUnique<HttpScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( SCRIPT_SERVICE()
            ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "EnvironmentScriptEmbedding" ), Helper::makeFactorableUnique<EnvironmentScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( SCRIPT_SERVICE()
            ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "NodeScriptEmbedding" ), Helper::makeFactorableUnique<NodeScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( SCRIPT_SERVICE()
            ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "ResourceScriptEmbedding" ), Helper::makeFactorableUnique<ResourceScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( SCRIPT_SERVICE()
            ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "EntityScriptEmbedding" ), Helper::makeFactorableUnique<EntityScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( SCRIPT_SERVICE()
            ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "SoundScriptEmbedding" ), Helper::makeFactorableUnique<SoundScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }
        
        if( SCRIPT_SERVICE()
            ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "AmplifierScriptEmbedding" ), Helper::makeFactorableUnique<AmplifierScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( SCRIPT_SERVICE()
            ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "EngineScriptEmbedding" ), Helper::makeFactorableUnique<EngineScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
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
            ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "ConstsScriptEmbedding" ) );

        SCRIPT_SERVICE()
            ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "MathScriptEmbedding" ) );

        SCRIPT_SERVICE()
            ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "HelperScriptEmbedding" ) );

        SCRIPT_SERVICE()
            ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "KernelScriptEmbedding" ) );

        SCRIPT_SERVICE()
            ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "HttpScriptEmbedding" ) );

        SCRIPT_SERVICE()
            ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "EnvironmentScriptEmbedding" ) );

        SCRIPT_SERVICE()
            ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "NodeScriptEmbedding" ) );

        SCRIPT_SERVICE()
            ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "ResourceScriptEmbedding" ) );        

        SCRIPT_SERVICE()
            ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "EntityScriptEmbedding" ) );

        SCRIPT_SERVICE()
            ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "SoundScriptEmbedding" ) );

        SCRIPT_SERVICE()
            ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "AmplifierScriptEmbedding" ) );

        SCRIPT_SERVICE()
            ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "EngineScriptEmbedding" ) );
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
            ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "GameScriptEmbedding" ), Helper::makeFactorableUnique<GameScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFramework::notifyBootstrapperFinalizeGame()
    {
        SCRIPT_SERVICE()
            ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "GameScriptEmbedding" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFramework::notifyEngineStopServices()
    {
        NOTIFICATION_NOTIFY( NOTIFICATOR_SCRIPT_EJECTING );
    }
    //////////////////////////////////////////////////////////////////////////
}
