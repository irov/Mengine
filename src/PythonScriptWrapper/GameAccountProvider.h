#   pragma once

#   include "Interface/AccountInterface.h"

#	include "Core/ServantBase.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class Game;
    //////////////////////////////////////////////////////////////////////////
    class GameAccountProvider
        : public ServantBase<AccountProviderInterface>
    {
    public:
        GameAccountProvider();

	public:
		void setGame(Game * _game);
		Game * getGame() const;

    protected:
        void onCreateAccount( const ConstString & _accountID, bool _global ) override;
        void onDeleteAccount( const ConstString & _accountID ) override;
        void onSelectAccount( const ConstString & _accountID ) override;
        void onUnselectAccount( const ConstString & _accountID ) override;

    protected:
        Game * m_game;
    };
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<GameAccountProvider> GameAccountProviderPtr;
	//////////////////////////////////////////////////////////////////////////
}