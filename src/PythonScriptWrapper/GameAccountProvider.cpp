#   include "GameAccountProvider.h"

#	include "Game.h"

//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    GameAccountProvider::GameAccountProvider()
        : m_game( nullptr )
    {
    }
	//////////////////////////////////////////////////////////////////////////
	void GameAccountProvider::setGame(Game * _game)
	{
		m_game = _game;
	}
	//////////////////////////////////////////////////////////////////////////
	Game * GameAccountProvider::getGame() const
	{
		return m_game;
	}
    //////////////////////////////////////////////////////////////////////////
    void GameAccountProvider::onCreateAccount( const ConstString & _accountID, bool _global )
    {
        EVENTABLE_METHOD( m_game, EVENT_GAME_CREATE_ACCOUNT )
            ->onGameCreateAccount( _accountID, _global );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAccountProvider::onDeleteAccount( const ConstString & _accountID )
    {
        EVENTABLE_METHOD( m_game, EVENT_GAME_DELETE_ACCOUNT )
            ->onGameDeleteAccount( _accountID );
        //EVENTABLE_CALL(m_serviceProvider, m_game, EVENT_GAME_DELETE_ACCOUNT)( _accountID );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAccountProvider::onSelectAccount( const ConstString & _accountID )
    {
        EVENTABLE_METHOD( m_game, EVENT_GAME_SELECT_ACCOUNT )
            ->onGameSelectAccount( _accountID );
        //EVENTABLE_CALL(m_serviceProvider, m_game, EVENT_GAME_SELECT_ACCOUNT)( _accountID );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAccountProvider::onUnselectAccount( const ConstString & _accountID )
    {
        EVENTABLE_METHOD( m_game, EVENT_GAME_UNSELECT_ACCOUNT )
            ->onGameUselectAccount( _accountID );
        //EVENTABLE_CALL(m_serviceProvider, m_game, EVENT_GAME_UNSELECT_ACCOUNT)( _accountID );
    }
}