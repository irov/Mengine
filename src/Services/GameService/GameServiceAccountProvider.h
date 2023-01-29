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
        void onCreateAccount( const ConstString & _accountId, bool _global ) override;
        void onDeleteAccount( const ConstString & _accountId ) override;
        void onSelectAccount( const ConstString & _accountId ) override;
        void onUnselectAccount( const ConstString & _accountId ) override;

    protected:
        EventablePtr m_eventable;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<GameServiceAccountProvider, AccountProviderInterface> GameAccountProviderPtr;
    //////////////////////////////////////////////////////////////////////////
}