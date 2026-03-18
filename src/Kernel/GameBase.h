#pragma once

#include "Interface/GameInterface.h"

#include "Kernel/DummyGameEventReceiver.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    class GameBase
        : public GameInterface
        , public DummyGameEventReceiver
        , public Factorable
    {
    public:
        GameBase();
        ~GameBase() override;

    public:
        void setName( const ConstString & _name ) override;
        const ConstString & getName() const override;

    public:
        bool initializeGame() override;
        void finalizeGame() override;

    public:
        bool isAvailableGame() const override;

    public:
        bool onGameInitialize() override;
        void onGameFinalize() override;
        void onGameInterruption() override;
        void onGameAnalyticsEvent( const AnalyticsEventInterfacePtr & _event ) override;
        void onGameAnalyticsScreenView( const ConstString & _screenType, const ConstString & _screenName ) override;
        void onGameAnalyticsFlush() override;

    protected:
        virtual bool _initializeGame();
        virtual void _finalizeGame();

    protected:
        virtual bool _availableGame() const;

    protected:
        ConstString m_name;

        bool m_initialize;
        bool m_available;
    };
}