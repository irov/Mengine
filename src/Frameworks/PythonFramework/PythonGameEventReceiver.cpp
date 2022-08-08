#include "PythonGameEventReceiver.h"

#include "Interface/PlayerServiceInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonGameEventReceiver::PythonGameEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonGameEventReceiver::~PythonGameEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameFullscreen( bool _fullscreen )
    {
        m_cb.call( _fullscreen );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameFixedContentResolution( bool _fixed )
    {
        m_cb.call( _fixed );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameFixedDisplayResolution( bool _fixed )
    {
        m_cb.call( _fixed );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameRenderViewport( const Viewport & _viewport, const Resolution & _contentResolution )
    {
        m_cb.call( _viewport, _contentResolution );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameViewport( const Viewport & _viewport, float _aspect )
    {
        m_cb.call( _viewport, _aspect );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonGameEventReceiver::onGameKey( const InputKeyEvent & _event )
    {
        mt::vec2f point( _event.x, _event.y );

        mt::vec2f wp;
        PLAYER_SERVICE()
            ->calcGlobalMouseWorldPosition( point, &wp );

        InputKeyEvent ev = _event;
        ev.x = wp.x;
        ev.y = wp.y;

        bool result = m_cb.call( ev );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonGameEventReceiver::onGameText( const InputTextEvent & _event )
    {
        mt::vec2f point( _event.x, _event.y );

        mt::vec2f wp;
        PLAYER_SERVICE()
            ->calcGlobalMouseWorldPosition( point, &wp );

        InputTextEvent ev = _event;
        ev.x = wp.x;
        ev.y = wp.y;

        bool result = m_cb.call( ev );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonGameEventReceiver::onGameMouseButton( const InputMouseButtonEvent & _event )
    {
        mt::vec2f point( _event.x, _event.y );

        mt::vec2f wp;
        PLAYER_SERVICE()
            ->calcGlobalMouseWorldPosition( point, &wp );

        InputMouseButtonEvent ev = _event;
        ev.x = wp.x;
        ev.y = wp.y;

        bool result = m_cb.call( ev );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonGameEventReceiver::onGameMouseButtonBegin( const InputMouseButtonEvent & _event )
    {
        mt::vec2f point( _event.x, _event.y );

        mt::vec2f wp;
        PLAYER_SERVICE()
            ->calcGlobalMouseWorldPosition( point, &wp );

        InputMouseButtonEvent ev = _event;
        ev.x = wp.x;
        ev.y = wp.y;

        bool result = m_cb.call( ev );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonGameEventReceiver::onGameMouseButtonEnd( const InputMouseButtonEvent & _event )
    {
        mt::vec2f point( _event.x, _event.y );

        mt::vec2f wp;
        PLAYER_SERVICE()
            ->calcGlobalMouseWorldPosition( point, &wp );

        InputMouseButtonEvent ev = _event;
        ev.x = wp.x;
        ev.y = wp.y;

        bool result = m_cb.call( ev );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonGameEventReceiver::onGameMouseMove( const InputMouseMoveEvent & _event )
    {
        mt::vec2f point( _event.x, _event.y );
        mt::vec2f delta( _event.dx, _event.dy );

        mt::vec2f wp;
        PLAYER_SERVICE()
            ->calcGlobalMouseWorldPosition( point, &wp );

        mt::vec2f wd;
        PLAYER_SERVICE()
            ->calcGlobalMouseWorldDelta( delta, &wd );

        InputMouseMoveEvent ev;
        ev.x = wp.x;
        ev.y = wp.y;
        ev.dx = wd.x;
        ev.dy = wd.y;

        bool result = m_cb.call( ev );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonGameEventReceiver::onGameMouseWheel( const InputMouseWheelEvent & _event )
    {
        bool result = m_cb.call( _event );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameAppMouseEnter( const InputMouseEnterEvent & _event )
    {
        m_cb.call( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameAppMouseLeave( const InputMouseLeaveEvent & _event )
    {
        m_cb.call( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameTimeFactor( float _timeFactor )
    {
        m_cb.call( _timeFactor );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonGameEventReceiver::onGamePreparation( bool _debug )
    {
        bool result = m_cb.call( _debug );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameRun()
    {
        m_cb.call();
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameInterruption()
    {
        m_cb.call();
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameStop()
    {
        m_cb.call();
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonGameEventReceiver::onGameInitialize()
    {
        bool result = m_cb.call();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameInitializeRenderResources()
    {
        m_cb.call();
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameFinalizeRenderResources()
    {
        m_cb.call();
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameAccountFinalize()
    {
        m_cb.call();
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameFinalize()
    {
        m_cb.call();
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameDestroy()
    {
        m_cb.call();
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameFocus( bool _focus )
    {
        m_cb.call( _focus );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameCreateDefaultAccount()
    {
        m_cb.call();
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameCreateGlobalAccount()
    {
        m_cb.call();
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameLoadAccounts()
    {
        m_cb.call();
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameCreateAccount( const ConstString & _accountID, bool _global )
    {
        m_cb.call( _accountID, _global );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameDeleteAccount( const ConstString & _accountID )
    {
        m_cb.call( _accountID );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameSelectAccount( const ConstString & _accountID )
    {
        m_cb.call( _accountID );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameUselectAccount( const ConstString & _accountID )
    {
        m_cb.call( _accountID );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameTurnSound( bool _turn )
    {
        m_cb.call( _turn );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameChangeSoundVolume( float _sound, float _music, float _voice )
    {
        m_cb.call( _sound, _music, _voice );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameCursorMode( bool _mode )
    {
        m_cb.call( _mode );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameUser( const ConstString & _event, const MapWParams & _params )
    {
        m_cb.call( _event, _params );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonGameEventReceiver::onGameClose()
    {
        bool result = m_cb.call();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameOverFillrate( double _fillrate, double _limit )
    {
        m_cb.call( _fillrate, _limit );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameFrameEnd()
    {
        m_cb.call();
    }
    //////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLATFORM_IOS
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameiOSApplicationDidBecomeActive()
    {
        m_cb.call();
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameiOSApplicationWillEnterForeground()
    {
        m_cb.call();
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameiOSApplicationDidEnterBackground()
    {
        m_cb.call();
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameiOSApplicationWillResignActive()
    {
        m_cb.call();
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameiOSApplicationWillTerminate()
    {
        m_cb.call();
    }
    //////////////////////////////////////////////////////////////////////////
#endif
}