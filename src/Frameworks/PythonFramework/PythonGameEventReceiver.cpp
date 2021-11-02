#include "PythonGameEventReceiver.h"

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
    bool PythonGameEventReceiver::onGameKey( EKeyCode _code, float _x, float _y, bool _isDown, bool _isRepeat )
    {
        return m_cb.call( (uint32_t)_code, _x, _y, _isDown, _isRepeat );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonGameEventReceiver::onGameText( WChar _key, float _x, float _y )
    {
        return m_cb.call( _key, _x, _y );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonGameEventReceiver::onGameMouseButton( ETouchCode _touchId, float _x, float _y, EMouseCode _code, bool _isDown )
    {
        return m_cb.call( _touchId, _x, _y, _code, _isDown );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonGameEventReceiver::onGameMouseButtonBegin( ETouchCode _touchId, float _x, float _y, EMouseCode _code, bool _isDown )
    {
        return m_cb.call( _touchId, _x, _y, _code, _isDown );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonGameEventReceiver::onGameMouseButtonEnd( ETouchCode _touchId, float _x, float _y, EMouseCode _code, bool _isDown )
    {
        return m_cb.call( _touchId, _x, _y, _code, _isDown );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonGameEventReceiver::onGameMouseMove( ETouchCode _touchId, float _x, float _y, float _dx, float _dy )
    {
        return m_cb.call( _touchId, _x, _y, _dx, _dy );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonGameEventReceiver::onGameMouseWheel( float _x, float _y, EWheelCode _code, int32_t _wheel )
    {
        return m_cb.call( _x, _y, _code, _wheel );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameAppMouseEnter( float _x, float _y )
    {
        m_cb.call( _x, _y );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameAppMouseLeave()
    {
        m_cb.call();
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameTimeFactor( float _timeFactor )
    {
        m_cb.call( _timeFactor );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonGameEventReceiver::onGamePreparation( bool _debug )
    {
        return m_cb.call( _debug );
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
        return m_cb.call();
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
        return m_cb.call();
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameOverFillrate( double _fillrate )
    {
        m_cb.call( _fillrate );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonGameEventReceiver::onGameFrameEnd()
    {
        m_cb.call();
    }
    //////////////////////////////////////////////////////////////////////////
}