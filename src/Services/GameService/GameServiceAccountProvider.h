#pragma once

#include "Interface/AccountProviderInterface.h"

#include "Kernel/Eventable.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class GameServiceAccountProvider
        : public AccountProviderInterface
        , public Factorable
    {
    public:
        GameServiceAccountProvider();
        ~GameServiceAccountProvider() override;

    public:
        void setEventable( const EventablePtr & _eventable );
        const EventablePtr & getEventable() const;

    protected:
        void onCreateAccount( const AccountInterfacePtr & _account, bool _global ) override;
        void onDeleteAccount( const AccountInterfacePtr & _account ) override;
        void onSelectAccount( const AccountInterfacePtr & _account ) override;
        void onUnselectAccount( const AccountInterfacePtr & _account ) override;

    protected:
        EventablePtr m_eventable;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<GameServiceAccountProvider, AccountProviderInterface> GameAccountProviderPtr;
    //////////////////////////////////////////////////////////////////////////
}