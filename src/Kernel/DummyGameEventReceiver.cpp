#include "Kernel/DummyGameEventReceiver.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DummyGameEventReceiver::DummyGameEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DummyGameEventReceiver::~DummyGameEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameFullscreen( bool _fullscreen )
    {
        MENGINE_UNUSED( _fullscreen );

        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameFixedContentResolution( bool _fixed )
    {
        MENGINE_UNUSED( _fixed );

        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameFixedDisplayResolution( bool _fixed )
    {
        MENGINE_UNUSED( _fixed );

        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameFixedViewportResolution( bool _fixed )
    {
        MENGINE_UNUSED( _fixed );

        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameRenderViewport( const Viewport & _viewport, const Resolution & _contentResolution )
    {
        MENGINE_UNUSED( _viewport );
        MENGINE_UNUSED( _contentResolution );

        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameViewport( const Viewport & _viewport, float _aspect )
    {
        MENGINE_UNUSED( _viewport );
        MENGINE_UNUSED( _aspect );

        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool DummyGameEventReceiver::onGameKey( const InputKeyEvent & _event )
    {
        MENGINE_UNUSED( _event );

        // Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DummyGameEventReceiver::onGameText( const InputTextEvent & _event )
    {
        MENGINE_UNUSED( _event );

        // Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DummyGameEventReceiver::onGameMouseButton( const InputMouseButtonEvent & _event )
    {
        MENGINE_UNUSED( _event );

        // Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DummyGameEventReceiver::onGameMouseButtonBegin( const InputMouseButtonEvent & _event )
    {
        MENGINE_UNUSED( _event );

        // Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DummyGameEventReceiver::onGameMouseButtonEnd( const InputMouseButtonEvent & _event )
    {
        MENGINE_UNUSED( _event );

        // Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DummyGameEventReceiver::onGameMouseMove( const InputMouseMoveEvent & _event )
    {
        MENGINE_UNUSED( _event );

        // Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DummyGameEventReceiver::onGameMouseWheel( const InputMouseWheelEvent & _event )
    {
        MENGINE_UNUSED( _event );

        // Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameAppMouseEnter( const InputMouseEnterEvent & _event )
    {
        MENGINE_UNUSED( _event );

        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameAppMouseLeave( const InputMouseLeaveEvent & _event )
    {
        MENGINE_UNUSED( _event );

        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameTimeFactorChange( float _timeFactor )
    {
        MENGINE_UNUSED( _timeFactor );

        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameSettingChange( const SettingInterfacePtr & _setting, const Char * _key )
    {
        MENGINE_UNUSED( _setting );
        MENGINE_UNUSED( _key );

        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool DummyGameEventReceiver::onGamePreparation( bool _debug )
    {
        MENGINE_UNUSED( _debug );

        // Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameRun()
    {
        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameStop()
    {
        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool DummyGameEventReceiver::onGameInitialize()
    {
        // Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameInitializeRenderResources()
    {
        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameFinalizeRenderResources()
    {
        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameAccountFinalize()
    {
        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameFinalize()
    {
        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameDestroy()
    {
        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameFocus( bool _focus )
    {
        MENGINE_UNUSED( _focus );

        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameCreateDefaultAccount()
    {
        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameCreateGlobalAccount()
    {
        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameLoadAccounts()
    {
        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameCreateAccount( const ConstString & _accountId, bool _global )
    {
        MENGINE_UNUSED( _accountId );
        MENGINE_UNUSED( _global );
        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameDeleteAccount( const ConstString & _accountId )
    {
        MENGINE_UNUSED( _accountId );

        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameSelectAccount( const ConstString & _accountId )
    {
        MENGINE_UNUSED( _accountId );

        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameUselectAccount( const ConstString & _accountId )
    {
        MENGINE_UNUSED( _accountId );
        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameTurnSound( bool _turn )
    {
        MENGINE_UNUSED( _turn );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameChangeSoundVolume( float _sound, float _music, float _voice )
    {
        MENGINE_UNUSED( _sound );
        MENGINE_UNUSED( _music );
        MENGINE_UNUSED( _voice );

        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameCursorMode( bool _mode )
    {
        MENGINE_UNUSED( _mode );

        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameUser( const ConstString & _event, const Params & _params )
    {
        MENGINE_UNUSED( _event );
        MENGINE_UNUSED( _params );

        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool DummyGameEventReceiver::onGameClose()
    {
        // Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameOverFillrate( double _fillrate, double _limit )
    {
        MENGINE_UNUSED( _fillrate );
        MENGINE_UNUSED( _limit );

        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameFrameEnd()
    {
        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameApplicationDidBecomeActive()
    {
        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameApplicationWillEnterForeground()
    {
        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameApplicationDidEnterBackground()
    {
        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameApplicationWillResignActive()
    {
        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameApplicationWillTerminate()
    {
        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
}