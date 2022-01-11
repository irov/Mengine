#pragma once

#include "Interface/EventationInterface.h"
#include "Interface/InputHandlerInterface.h"

#include "Kernel/Params.h"
#include "Kernel/KeyCode.h"
#include "Kernel/MouseCode.h"
#include "Kernel/WheelCode.h"
#include "Kernel/TouchCode.h"
#include "Kernel/Viewport.h"
#include "Kernel/Resolution.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EGameEventFlags
    {
        EVENT_GAME_FULLSCREEN = 0,
        EVENT_GAME_FIXED_CONTENT_RESOLUTION,
        EVENT_GAME_FIXED_DISPLAY_RESOLUTION,
        EVENT_GAME_RENDER_VIEWPORT,
        EVENT_GAME_VIEWPORT,
        EVENT_GAME_KEY,
        EVENT_GAME_TEXT,
        EVENT_GAME_MOUSE_BUTTON,
        EVENT_GAME_MOUSE_BUTTON_BEGIN,
        EVENT_GAME_MOUSE_BUTTON_END,
        EVENT_GAME_MOUSE_MOVE,
        EVENT_GAME_MOUSE_WHEEL,
        EVENT_GAME_APP_MOUSE_ENTER,
        EVENT_GAME_APP_MOUSE_LEAVE,
        EVENT_GAME_ON_TIME_FACTOR,
        EVENT_GAME_PREPARATION,
        EVENT_GAME_RUN,
        EVENT_GAME_INTERRUPTION,
        EVENT_GAME_STOP,
        EVENT_GAME_INITIALIZE,
        EVENT_GAME_INITIALIZE_RENDER_RESOURCES,
        EVENT_GAME_FINALIZE_RENDER_RESOURCES,
        EVENT_GAME_ACCOUNT_FINALIZE,
        EVENT_GAME_FINALIZE,
        EVENT_GAME_DESTROY,
        EVENT_GAME_FOCUS,
        EVENT_GAME_CREATE_DEFAULT_ACCOUNT,
        EVENT_GAME_CREATE_GLOBAL_ACCOUNT,
        EVENT_GAME_LOAD_ACCOUNTS,
        EVENT_GAME_CREATE_ACCOUNT,
        EVENT_GAME_DELETE_ACCOUNT,
        EVENT_GAME_SELECT_ACCOUNT,
        EVENT_GAME_UNSELECT_ACCOUNT,
        EVENT_GAME_TURN_SOUND,
        EVENT_GAME_CHANGE_SOUND_VOLUME,
        EVENT_GAME_CURSOR_MODE,
        EVENT_GAME_USER,
        EVENT_GAME_CLOSE,
        EVENT_GAME_OVER_FILLRATE,
        EVENT_GAME_FRAME_END,

        __EVENT_GAME_LAST__
    };
    //////////////////////////////////////////////////////////////////////////
    class GameEventReceiverInterface
        : public EventReceiverInterface
    {
    public:
        virtual void onGameFullscreen( bool _fullscreen ) = 0;
        virtual void onGameFixedContentResolution( bool _fixed ) = 0;
        virtual void onGameFixedDisplayResolution( bool _fixed ) = 0;
        virtual void onGameRenderViewport( const Viewport & _viewport, const Resolution & _contentResolution ) = 0;
        virtual void onGameViewport( const Viewport & _viewport, float _aspect ) = 0;
        virtual bool onGameKey( const InputKeyEvent & _event ) = 0;
        virtual bool onGameText( WChar _key, float _x, float _y ) = 0;
        virtual bool onGameMouseButton( ETouchCode _touchId, float _x, float _y, EMouseCode _button, bool _isDown ) = 0;
        virtual bool onGameMouseButtonBegin( ETouchCode _touchId, float _x, float _y, EMouseCode _button, bool _isDown ) = 0;
        virtual bool onGameMouseButtonEnd( ETouchCode _touchId, float _x, float _y, EMouseCode _button, bool _isDown ) = 0;
        virtual bool onGameMouseMove( ETouchCode _touchId, float _x, float _y, float _dx, float _dy ) = 0;
        virtual bool onGameMouseWheel( float _x, float _y, EWheelCode _button, int32_t _wheel ) = 0;
        virtual void onGameAppMouseEnter( float _x, float _y ) = 0;
        virtual void onGameAppMouseLeave() = 0;
        virtual void onGameTimeFactor( float _timeFactor ) = 0;
        virtual bool onGamePreparation( bool _debug ) = 0;
        virtual void onGameRun() = 0;
        virtual void onGameInterruption() = 0;
        virtual void onGameStop() = 0;
        virtual bool onGameInitialize() = 0;
        virtual void onGameInitializeRenderResources() = 0;
        virtual void onGameFinalizeRenderResources() = 0;
        virtual void onGameAccountFinalize() = 0;
        virtual void onGameFinalize() = 0;
        virtual void onGameDestroy() = 0;
        virtual void onGameFocus( bool _focus ) = 0;
        virtual void onGameCreateDefaultAccount() = 0;
        virtual void onGameCreateGlobalAccount() = 0;
        virtual void onGameLoadAccounts() = 0;
        virtual void onGameCreateAccount( const ConstString & _accountID, bool _global ) = 0;
        virtual void onGameDeleteAccount( const ConstString & _accountID ) = 0;
        virtual void onGameSelectAccount( const ConstString & _accountID ) = 0;
        virtual void onGameUselectAccount( const ConstString & _accountID ) = 0;
        virtual void onGameTurnSound( bool _turn ) = 0;
        virtual void onGameChangeSoundVolume( float _sound, float _music, float _voice ) = 0;
        virtual void onGameCursorMode( bool _mode ) = 0;
        virtual void onGameUser( const ConstString & _event, const MapWParams & _params ) = 0;
        virtual bool onGameClose() = 0;
        virtual void onGameOverFillrate( double _fillrate ) = 0;
        virtual void onGameFrameEnd() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_FULLSCREEN );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_FIXED_CONTENT_RESOLUTION );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_FIXED_DISPLAY_RESOLUTION );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_RENDER_VIEWPORT );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_VIEWPORT );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_KEY );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_TEXT );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_MOUSE_BUTTON );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_MOUSE_BUTTON_BEGIN );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_MOUSE_BUTTON_END );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_MOUSE_MOVE );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_MOUSE_WHEEL );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_APP_MOUSE_ENTER );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_APP_MOUSE_LEAVE );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_ON_TIME_FACTOR );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_PREPARATION );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_RUN );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_INTERRUPTION );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_STOP );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_INITIALIZE );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_INITIALIZE_RENDER_RESOURCES );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_ACCOUNT_FINALIZE );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_FINALIZE );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_FINALIZE_RENDER_RESOURCES );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_DESTROY );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_FOCUS );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_CREATE_DEFAULT_ACCOUNT );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_CREATE_GLOBAL_ACCOUNT );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_LOAD_ACCOUNTS );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_CREATE_ACCOUNT );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_DELETE_ACCOUNT );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_SELECT_ACCOUNT );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_TURN_SOUND );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_UNSELECT_ACCOUNT );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_CHANGE_SOUND_VOLUME );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_CURSOR_MODE );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_USER );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_CLOSE );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_OVER_FILLRATE );
    EVENTATION_TYPEID( GameEventReceiverInterface, EVENT_GAME_FRAME_END );
    //////////////////////////////////////////////////////////////////////////
}