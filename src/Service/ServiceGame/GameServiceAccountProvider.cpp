#include "GameServiceAccountProvider.h"

#include "Interface/GameServiceInterface.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    GameServiceAccountProvider::GameServiceAccountProvider()
    {
    }
	//////////////////////////////////////////////////////////////////////////
	GameServiceAccountProvider::~GameServiceAccountProvider()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void GameServiceAccountProvider::setEventable( const EventablePtr & _eventable )
    {
        m_eventable = _eventable;
    }
    //////////////////////////////////////////////////////////////////////////
	const EventablePtr & GameServiceAccountProvider::getEventable() const
    {
        return m_eventable;
    }
    //////////////////////////////////////////////////////////////////////////
    void GameServiceAccountProvider::onCreateAccount( const ConstString & _accountID, bool _global )
    {
        EVENTABLE_METHODT( m_eventable, EVENT_GAME_CREATE_ACCOUNT, GameEventReceiver )
            ->onGameCreateAccount( _accountID, _global );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameServiceAccountProvider::onDeleteAccount( const ConstString & _accountID )
    {
        EVENTABLE_METHODT( m_eventable, EVENT_GAME_DELETE_ACCOUNT, GameEventReceiver )
            ->onGameDeleteAccount( _accountID );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameServiceAccountProvider::onSelectAccount( const ConstString & _accountID )
    {
        EVENTABLE_METHODT( m_eventable, EVENT_GAME_SELECT_ACCOUNT, GameEventReceiver )
            ->onGameSelectAccount( _accountID );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameServiceAccountProvider::onUnselectAccount( const ConstString & _accountID )
    {
        EVENTABLE_METHODT( m_eventable, EVENT_GAME_UNSELECT_ACCOUNT, GameEventReceiver )
            ->onGameUselectAccount( _accountID );
    }
}