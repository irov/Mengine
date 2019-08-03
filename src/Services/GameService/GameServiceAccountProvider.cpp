#include "GameServiceAccountProvider.h"

#include "Interface/GameServiceInterface.h"

#include "Kernel/EventableHelper.h"

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
        EVENTABLE_OTHER_METHODT( m_eventable, EVENT_GAME_CREATE_ACCOUNT, GameEventReceiverInterface )
            ->onGameCreateAccount( _accountID, _global );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameServiceAccountProvider::onDeleteAccount( const ConstString & _accountID )
    {
        EVENTABLE_OTHER_METHODT( m_eventable, EVENT_GAME_DELETE_ACCOUNT, GameEventReceiverInterface )
            ->onGameDeleteAccount( _accountID );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameServiceAccountProvider::onSelectAccount( const ConstString & _accountID )
    {
        EVENTABLE_OTHER_METHODT( m_eventable, EVENT_GAME_SELECT_ACCOUNT, GameEventReceiverInterface )
            ->onGameSelectAccount( _accountID );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameServiceAccountProvider::onUnselectAccount( const ConstString & _accountID )
    {
        EVENTABLE_OTHER_METHODT( m_eventable, EVENT_GAME_UNSELECT_ACCOUNT, GameEventReceiverInterface )
            ->onGameUselectAccount( _accountID );
    }
}