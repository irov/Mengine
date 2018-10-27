#pragma once

#include "Interface/AccountInterface.h"

#include "Kernel/Eventable.h"
#include "Kernel/ServantBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class GameServiceAccountProvider
        : public ServantBase<AccountProviderInterface>
    {
    public:
        GameServiceAccountProvider();
		~GameServiceAccountProvider() override;

    public:
        void setEventable( const EventablePtr & _eventable );
		const EventablePtr & getEventable() const;

    protected:
        void onCreateAccount( const ConstString & _accountID, bool _global ) override;
        void onDeleteAccount( const ConstString & _accountID ) override;
        void onSelectAccount( const ConstString & _accountID ) override;
        void onUnselectAccount( const ConstString & _accountID ) override;

    protected:
		EventablePtr m_eventable;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<GameServiceAccountProvider> GameAccountProviderPtr;
    //////////////////////////////////////////////////////////////////////////
}