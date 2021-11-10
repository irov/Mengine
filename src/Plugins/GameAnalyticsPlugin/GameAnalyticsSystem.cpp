#include "GameAnalyticsSystem.h"

#include "Interface/NotificationServiceInterface.h"

#include "GameAnalytics.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AnalyticsSystem, Mengine::GameAnalyticsSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    GameAnalyticsSystem::GameAnalyticsSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    GameAnalyticsSystem::~GameAnalyticsSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool GameAnalyticsSystem::_initializeService()
    {
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_GAME, &GameAnalyticsSystem::notifyBootstrapperInitializeGame_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_FINALIZE_GAME, &GameAnalyticsSystem::notifyBootstrapperFinalizeGame_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAnalyticsSystem::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_GAME );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_FINALIZE_GAME );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAnalyticsSystem::startProgressionEvent( const Char * _name )
    {
        gameanalytics::GameAnalytics::addProgressionEvent( gameanalytics::EGAProgressionStatus::Start, _name, "", "" );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAnalyticsSystem::completeProgressionEvent( const Char * _name )
    {
        gameanalytics::GameAnalytics::addProgressionEvent( gameanalytics::EGAProgressionStatus::Complete, _name, "", "" );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAnalyticsSystem::failProgressionEvent( const Char * _name )
    {
        gameanalytics::GameAnalytics::addProgressionEvent( gameanalytics::EGAProgressionStatus::Fail, _name, "", "" );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAnalyticsSystem::addDesignEvent( const Char * _event )
    {
        gameanalytics::GameAnalytics::addDesignEvent( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAnalyticsSystem::notifyBootstrapperInitializeGame_()
    {
        gameanalytics::GameAnalytics::startSession();
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAnalyticsSystem::notifyBootstrapperFinalizeGame_()
    {
        gameanalytics::GameAnalytics::endSession();
    }
    //////////////////////////////////////////////////////////////////////////
}
