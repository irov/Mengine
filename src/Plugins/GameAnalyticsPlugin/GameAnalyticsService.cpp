#include "GameAnalyticsService.h"

#include "GameAnalytics.h"

#include "Kernel/NotificationHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( GameAnalyticsService, Mengine::GameAnalyticsService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    GameAnalyticsService::GameAnalyticsService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    GameAnalyticsService::~GameAnalyticsService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool GameAnalyticsService::_initializeService()
    {
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_GAME, &GameAnalyticsService::notifyBootstrapperInitializeGame_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_FINALIZE_GAME, &GameAnalyticsService::notifyBootstrapperFinalizeGame_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAnalyticsService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_GAME );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_FINALIZE_GAME );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAnalyticsService::startProgressionEvent( const Char * _name )
    {
        gameanalytics::GameAnalytics::addProgressionEvent( gameanalytics::EGAProgressionStatus::Start, _name, "", "" );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAnalyticsService::completeProgressionEvent( const Char * _name )
    {
        gameanalytics::GameAnalytics::addProgressionEvent( gameanalytics::EGAProgressionStatus::Complete, _name, "", "" );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAnalyticsService::failProgressionEvent( const Char * _name )
    {
        gameanalytics::GameAnalytics::addProgressionEvent( gameanalytics::EGAProgressionStatus::Fail, _name, "", "" );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAnalyticsService::addDesignEvent( const Char * _event )
    {
        gameanalytics::GameAnalytics::addDesignEvent( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAnalyticsService::notifyBootstrapperInitializeGame_()
    {
        gameanalytics::GameAnalytics::startSession();
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAnalyticsService::notifyBootstrapperFinalizeGame_()
    {
        gameanalytics::GameAnalytics::endSession();
    }
    //////////////////////////////////////////////////////////////////////////
}
