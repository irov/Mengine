#include "GameAnalyticsSystem.h"

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
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAnalyticsSystem::_finalizeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAnalyticsSystem::startSession()
    {
        gameanalytics::GameAnalytics::startSession();
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAnalyticsSystem::endSession()
    {
        gameanalytics::GameAnalytics::endSession();
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
}
