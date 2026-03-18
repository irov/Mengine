#include "GameBase.h"

#include "Kernel/Assertion.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    GameBase::GameBase()
        : m_initialize( false )
        , m_available( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    GameBase::~GameBase()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void GameBase::setName( const ConstString & _name )
    {
        m_name = _name;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & GameBase::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GameBase::isAvailableGame() const
    {
        return m_available;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GameBase::_availableGame() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GameBase::initializeGame()
    {
        m_available = this->_availableGame();

        if( m_available == false )
        {
            return false;
        }

        m_initialize = this->_initializeGame();

        return m_initialize;
    }
    //////////////////////////////////////////////////////////////////////////
    void GameBase::finalizeGame()
    {
        if( m_available == false )
        {
            return;
        }

        MENGINE_ASSERTION_FATAL( m_initialize == true, "game '%s' is not initialized"
            , m_name.c_str()
        );

        this->_finalizeGame();
    }
    //////////////////////////////////////////////////////////////////////////
    bool GameBase::onGameInitialize()
    {
        return this->initializeGame();
    }
    //////////////////////////////////////////////////////////////////////////
    void GameBase::onGameFinalize()
    {
        this->finalizeGame();
    }
    //////////////////////////////////////////////////////////////////////////
    void GameBase::onGameInterruption()
    {
        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void GameBase::onGameAnalyticsEvent( const AnalyticsEventInterfacePtr & _event )
    {
        MENGINE_UNUSED( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameBase::onGameAnalyticsScreenView( const ConstString & _screenType, const ConstString & _screenName )
    {
        MENGINE_UNUSED( _screenType );
        MENGINE_UNUSED( _screenName );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameBase::onGameAnalyticsFlush()
    {
        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool GameBase::_initializeGame()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GameBase::_finalizeGame()
    {
        // Empty
    }
}