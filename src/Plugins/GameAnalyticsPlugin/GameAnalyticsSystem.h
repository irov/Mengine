#pragma once

#include "Interface/AnalyticsSystemInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class GameAnalyticsSystem
        : public ServiceBase<AnalyticsSystemInterface>
    {
    public:
        GameAnalyticsSystem();
        ~GameAnalyticsSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void startSession() override;
        void endSession() override;

    public:
        void startProgressionEvent( const Char * _name ) override;
        void completeProgressionEvent( const Char * _name ) override;
        void failProgressionEvent( const Char * _name ) override;
    };
}
