#pragma once

#include "Interface/GameEventReceiverInterface.h"

#include "Environment/Python/PythonEventReceiver.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class PythonGameEventReceiver
        : public GameEventReceiverInterface
        , public PythonEventReceiver
        , public Factorable
    {
    public:
        PythonGameEventReceiver();
        ~PythonGameEventReceiver() override;

    protected:
        void onGameFullscreen( bool _fullscreen ) override;
        void onGameFixedContentResolution( bool _fixed ) override;
        void onGameFixedDisplayResolution( bool _fixed ) override;
        void onGameRenderViewport( const Viewport & _viewport, const Resolution & _contentResolution ) override;
        void onGameViewport( const Viewport & _viewport, float _aspect ) override;
        bool onGameKey( const InputKeyEvent & _event ) override;
        bool onGameText( WChar _key, float _x, float _y ) override;
        bool onGameMouseButton( ETouchCode _touchId, float _x, float _y, EMouseCode _code, bool _isDown ) override;
        bool onGameMouseButtonBegin( ETouchCode _touchId, float _x, float _y, EMouseCode _code, bool _isDown ) override;
        bool onGameMouseButtonEnd( ETouchCode _touchId, float _x, float _y, EMouseCode _code, bool _isDown ) override;
        bool onGameMouseMove( ETouchCode _touchId, float _x, float _y, float _dx, float _dy ) override;
        bool onGameMouseWheel( float _x, float _y, EWheelCode _code, int32_t _wheel ) override;
        void onGameAppMouseEnter( float _x, float _y ) override;
        void onGameAppMouseLeave() override;
        void onGameTimeFactor( float _timeFactor ) override;
        bool onGamePreparation( bool _debug ) override;
        void onGameRun() override;
        void onGameInterruption() override;
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