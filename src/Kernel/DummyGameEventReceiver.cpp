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
    bool DummyGameEventReceiver::onGameKey( EKeyCode _code, float _x, float _y, bool _isDown, bool _isRepeat )
    {
        MENGINE_UNUSED( _code );
        MENGINE_UNUSED( _x );
        MENGINE_UNUSED( _y );
        MENGINE_UNUSED( _isDown );
        MENGINE_UNUSED( _isRepeat );
        // Empty
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DummyGameEventReceiver::onGameText( WChar _key, float _x, float _y )
    {
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _x );
        MENGINE_UNUSED( _y );
        // Empty
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DummyGameEventReceiver::onGameMouseButton( ETouchCode _touchId, float _x, float _y, EMouseCode _button, bool _isDown )
    {
        MENGINE_UNUSED( _touchId );
        MENGINE_UNUSED( _x );
        MENGINE_UNUSED( _y );
        MENGINE_UNUSED( _button );
        MENGINE_UNUSED( _isDown );
        // Empty
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DummyGameEventReceiver::onGameMouseButtonBegin( ETouchCode _touchId, float _x, float _y, EMouseCode _button, bool _isDown )
    {
        MENGINE_UNUSED( _touchId );
        MENGINE_UNUSED( _x );
        MENGINE_UNUSED( _y );
        MENGINE_UNUSED( _button );
        MENGINE_UNUSED( _isDown );
        // Empty
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DummyGameEventReceiver::onGameMouseButtonEnd( ETouchCode _touchId, float _x, float _y, EMouseCode _button, bool _isDown )
    {
        MENGINE_UNUSED( _touchId );
        MENGINE_UNUSED( _x );
        MENGINE_UNUSED( _y );
        MENGINE_UNUSED( _button );
        MENGINE_UNUSED( _isDown );
        // Empty
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DummyGameEventReceiver::onGameMouseMove( ETouchCode _touchId, float _x, float _y, float _dx, float _dy )
    {
        MENGINE_UNUSED( _touchId );
        MENGINE_UNUSED( _x );
        MENGINE_UNUSED( _y );
        MENGINE_UNUSED( _dx );
        MENGINE_UNUSED( _dy );
        // Empty
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DummyGameEventReceiver::onGameMouseWheel( EMouseCode _button, float _x, float _y, int32_t _wheel )
    {
        MENGINE_UNUSED( _button );
        MENGINE_UNUSED( _x );
        MENGINE_UNUSED( _y );
        MENGINE_UNUSED( _wheel );
        // Empty
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameAppMouseEnter( float _x, float _y )
    {
        MENGINE_UNUSED( _x );
        MENGINE_UNUSED( _y );
        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameAppMouseLeave()
    {
        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameTimeFactor( float _timeFactor )
    {
        MENGINE_UNUSED( _timeFactor );
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
    void DummyGameEventReceiver::onGameCreateAccount( const ConstString & _accountID, bool _global )
    {
        MENGINE_UNUSED( _accountID );
        MENGINE_UNUSED( _global );
        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameDeleteAccount( const ConstString & _accountID )
    {
        MENGINE_UNUSED( _accountID );
        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameSelectAccount( const ConstString & _accountID )
    {
        MENGINE_UNUSED( _accountID );
        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameUselectAccount( const ConstString & _accountID )
    {
        MENGINE_UNUSED( _accountID );
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
    void DummyGameEventReceiver::onGameUser( const ConstString & _event, const MapWParams & _params )
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
    void DummyGameEventReceiver::onGameOverFillrate( double _fillrate )
    {
        MENGINE_UNUSED( _fillrate );
        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyGameEventReceiver::onGameFrameEnd()
    {
        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
