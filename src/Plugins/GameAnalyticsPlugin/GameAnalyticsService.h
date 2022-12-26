#pragma once

#include "GameAnalyticsInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class GameAnalyticsService
        : public ServiceBase<GameAnalyticsServiceInterface>
    {
    public:
        GameAnalyticsService();
        ~GameAnalyticsService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void startProgressionEvent( const Char * _name ) override;
        void completeProgressionEvent( const Char * _name ) override;
        void failProgressionEvent( const Char * _name ) override;

    public:
        void addDesignEvent( const Char * _event ) override;

    protected:
        void notifyBootstrapperInitializeGame_();
        void notifyBootstrapperFinalizeGame_();
    };
}
