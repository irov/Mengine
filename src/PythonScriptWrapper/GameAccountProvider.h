#   pragma once

#   include "Interface/AccountInterface.h"

#   include "Game.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    class GameAccountProvider
        : public AccountProviderInterface
    {
    public:
        GameAccountProvider( ServiceProviderInterface * _serviceProvider, Game * _game );

    protected:
        void onCreateAccount( const WString & _accountID ) override;
        void onDeleteAccount( const WString & _accountID ) override;
        void onSelectAccount( const WString & _accountID ) override;
        void onUnselectAccount( const WString & _accountID ) override;

    protected:
        ServiceProviderInterface * m_serviceProvider;
        Game * m_game;
    };
}