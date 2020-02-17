#include "PythonFramework.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/ScriptProviderServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"

#include "ConstsScriptEmbedding.h"
#include "EngineScriptEmbedding.h"
#include "EntityScriptEmbedding.h"
#include "GameScriptEmbedding.h"
#include "HelperScriptEmbedding.h"
#include "MathScriptEmbedding.h"
#include "NodeScriptEmbedding.h"
#include "SoundScriptEmbedding.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"

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
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFramework::_finalizeFramework()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonFramework::_runFramework()
    {
        if( ADD_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "ConstsScriptEmbedding" ), Helper::makeFactorableUnique<ConstsScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( ADD_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "MathScriptEmbedding" ), Helper::makeFactorableUnique<MathScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( ADD_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "HelperScriptEmbedding" ), Helper::makeFactorableUnique<HelperScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( ADD_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "NodeScriptEmbedding" ), Helper::makeFactorableUnique<NodeScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( ADD_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "EntityScriptEmbedding" ), Helper::makeFactorableUnique<EntityScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( ADD_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "SoundScriptEmbedding" ), Helper::makeFactorableUnique<SoundScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( ADD_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "EngineScriptEmbedding" ), Helper::makeFactorableUnique<EngineScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_NOTIFY( NOTIFICATOR_SCRIPT_EMBEDDING_END );

        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_GAME, this, &PythonFramework::notifyBootstrapperInitializeGame );
        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_BOOTSTRAPPER_FINALIZE_GAME, this, &PythonFramework::notifyBootstrapperFinalizeGame );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFramework::_stopFramework()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_GAME );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_FINALIZE_GAME );

        REMOVE_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "ConstsScriptEmbedding" ) );
        REMOVE_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "MathScriptEmbedding" ) );
        REMOVE_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "HelperScriptEmbedding" ) );
        REMOVE_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "NodeScriptEmbedding" ) );
        REMOVE_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "EntityScriptEmbedding" ) );
        REMOVE_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "SoundScriptEmbedding" ) );
        REMOVE_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "EngineScriptEmbedding" ) );        

        NOTIFICATION_NOTIFY( NOTIFICATOR_SCRIPT_EJECTING );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFramework::notifyBootstrapperInitializeGame()
    {
        ADD_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "GameScriptEmbedding" ), Helper::makeFactorableUnique<GameScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFramework::notifyBootstrapperFinalizeGame()
    {
        REMOVE_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "GameScriptEmbedding" ) );
    }
}
