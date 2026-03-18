#include "GameServiceAccountProvider.h"

#include "Interface/GameEventReceiverInterface.h"

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
    void GameServiceAccountProvider::onCreateAccount( const AccountInterfacePtr & _account, bool _global )
    {
        EVENTABLE_OTHER_METHODT( m_eventable, EVENT_GAME_CREATE_ACCOUNT, GameEventReceiverInterface )
            ->onGameCreateAccount( _account, _global );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameServiceAccountProvider::onDeleteAccount( const AccountInterfacePtr & _account )
    {
        EVENTABLE_OTHER_METHODT( m_eventable, EVENT_GAME_DELETE_ACCOUNT, GameEventReceiverInterface )
            ->onGameDeleteAccount( _account );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameServiceAccountProvider::onSelectAccount( const AccountInterfacePtr & _account )
    {
        EVENTABLE_OTHER_METHODT( m_eventable, EVENT_GAME_SELECT_ACCOUNT, GameEventReceiverInterface )
            ->onGameSelectAccount( _account );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameServiceAccountProvider::onUnselectAccount( const AccountInterfacePtr & _account )
    {
        EVENTABLE_OTHER_METHODT( m_eventable, EVENT_GAME_UNSELECT_ACCOUNT, GameEventReceiverInterface )
            ->onGameUnselectAccount( _account );
    }
    //////////////////////////////////////////////////////////////////////////
}