#pragma once

#include "Interface/GameServiceInterface.h"

namespace Mengine
{
    class DummyGameEventReceiver
        : public GameEventReceiverInterface
    {
    public:
        DummyGameEventReceiver();
        ~DummyGameEventReceiver() override;

    public:
        void onGameFullscreen( bool _fullscreen ) override;
        void onGameFixedContentResolution( bool _fixed ) override;
        void onGameFixedDisplayResolution( bool _fixed ) override;
        void onGameRenderViewport( const Viewport & _viewport, const Resolution & _contentResolution ) override;
        void onGameViewport( const Viewport & _viewport, float _aspect ) override;
        bool onGameKey( EKeyCode _code, float _x, float _y, bool _isDown, bool _isRepeat ) override;
        bool onGameText( WChar _key, float _x, float _y ) override;
        bool onGameMouseButton( ETouchCode _touchId, float _x, float _y, EMouseCode _button, bool _isDown ) override;
        bool onGameMouseButtonBegin( ETouchCode _touchId, float _x, float _y, EMouseCode _button, bool _isDown ) override;
        bool onGameMouseButtonEnd( ETouchCode _touchId, float _x, float _y, EMouseCode _button, bool _isDown ) override;
        bool onGameMouseMove( ETouchCode _touchId, float _x, float _y, float _dx, float _dy ) override;
        bool onGameMouseWheel( EMouseCode _button, float _x, float _y, int32_t _wheel ) override;
        void onGameAppMouseEnter( float _x, float _y ) override;
        void onGameAppMouseLeave() override;
        void onGameTimeFactor( float _timeFactor ) override;
        bool onGamePreparation( bool _debug ) override;
        void onGameRun() override;
        void onGameStop() override;
        bool onGameInitialize() override;
        void onGameInitializeRenderResources() override;
        void onGameFinalizeRenderResources() override;
        void onGameAccountFinalize() override;
        void onGameFinalize() override;
        void onGameDestroy() override;
        void onGameFocus( bool _focus ) override;
        void onGameCreateDefaultAccount() override;
        void onGameCreateGlobalAccount() override;
        void onGameLoadAccounts() override;
        void onGameCreateAccount( const ConstString & _accountID, bool _global ) override;
        void onGameDeleteAccount( const ConstString & _accountID ) override;
        void onGameSelectAccount( const ConstString & _accountID ) override;
        void onGameUselectAccount( const ConstString & _accountID ) override;
        void onGameTurnSound( bool _turn ) override;
        void onGameChangeSoundVolume( float _sound, float _music, float _voice ) override;
        void onGameCursorMode( bool _mode ) override;
        void onGameUser( const ConstString & _event, const MapWParams & _params ) override;
        bool onGameClose() override;
        void onGameOverFillrate( double _fillrate ) override;
        void onGameFrameEnd() override;
    };
}