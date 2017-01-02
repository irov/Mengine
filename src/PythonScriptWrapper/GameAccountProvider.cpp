#   include "GameAccountProvider.h"

//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    GameAccountProvider::GameAccountProvider( ServiceProviderInterface * _serviceProvider, Game * _game )
        : m_serviceProvider( _serviceProvider )
        , m_game( _game )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAccountProvider::onCreateAccount( const WString & _accountID )
    {
        EVENTABLE_METHOD( m_game, EVENT_GAME_CREATE_ACCOUNT )
            ->onGameCreateAccount( _accountID );
        //EVENTABLE_CALL(m_serviceProvider, m_game, EVENT_GAME_CREATE_ACCOUNT)( _accountID );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAccountProvider::onDeleteAccount( const WString & _accountID )
    {
        EVENTABLE_METHOD( m_game, EVENT_GAME_DELETE_ACCOUNT )
            ->onGameDeleteAccount( _accountID );
        //EVENTABLE_CALL(m_serviceProvider, m_game, EVENT_GAME_DELETE_ACCOUNT)( _accountID );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAccountProvider::onSelectAccount( const WString & _accountID )
    {
        EVENTABLE_METHOD( m_game, EVENT_GAME_SELECT_ACCOUNT )
            ->onGameSelectAccount( _accountID );
        //EVENTABLE_CALL(m_serviceProvider, m_game, EVENT_GAME_SELECT_ACCOUNT)( _accountID );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameAccountProvider::onUnselectAccount( const WString & _accountID )
    {
        EVENTABLE_METHOD( m_game, EVENT_GAME_UNSELECT_ACCOUNT )
            ->onGameUselectAccount( _accountID );
        //EVENTABLE_CALL(m_serviceProvider, m_game, EVENT_GAME_UNSELECT_ACCOUNT)( _accountID );
    }
}