#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    class GameAnalyticsServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "GameAnalyticsService" )

    public:
        virtual void startProgressionEvent( const Char * _name ) = 0;
        virtual void completeProgressionEvent( const Char * _name ) = 0;
        virtual void failProgressionEvent( const Char * _name ) = 0;

    public:
        virtual void addDesignEvent( const Char * _event ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define GAME_ANALYTICS_SERVICE()\
    ((Mengine::GameAnalyticsServiceInterface *)SERVICE_GET(Mengine::GameAnalyticsServiceInterface))
//////////////////////////////////////////////////////////////////////////
