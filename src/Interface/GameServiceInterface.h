#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/UpdationInterface.h"
#include "Interface/EventationInterface.h"
#include "Interface/InputHandlerInterface.h"

#include "Kernel/Params.h"
#include "Kernel/KeyCode.h"
#include "Kernel/Eventable.h"
#include "Kernel/Viewport.h"
#include "Kernel/Resolution.h"

#ifndef MENGINE_GAME_PARAM_MAXVALUE
#define MENGINE_GAME_PARAM_MAXVALUE 1024
#endif

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
        EVENT_GAME_ON_TIMING_FACTOR,
        EVENT_GAME_PREPARATION,
        EVENT_GAME_RUN,
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
        EVENT_GAME_CHANGE_SOUND_VOLUME,
        EVENT_GAME_CURSOR_MODE,
        EVENT_GAME_USER,
        EVENT_GAME_CLOSE,
        EVENT_GAME_OVER_FILLRATE,
        EVENT_GAME_FRAME_END,
    };
    //////////////////////////////////////////////////////////////////////////
    class GameEventReceiver
        : public EventReceiverInterface
    {
    public:
        virtual void onGameFullscreen( bool _fullscreen ) = 0;
        virtual void onGameFixedContentResolution( bool _fixed ) = 0;
        virtual void onGameFixedDisplayResolution( bool _fixed ) = 0;
        virtual void onGameRenderViewport( const Viewport & _viewport, const Resolution & _contentResolution ) = 0;
        virtual void onGameViewport( const Viewport & _viewport, float _aspect ) = 0;
        virtual bool onGameKey( EKeyCode _code, float _x, float _y, bool _isDown, bool _isRepeat ) = 0;
        virtual bool onGameText( WChar _key, float _x, float _y ) = 0;
        virtual bool onGameMouseButton( uint32_t _touchId, float _x, float _y, uint32_t _button, bool _isDown ) = 0;
        virtual bool onGameMouseButtonBegin( uint32_t _touchId, float _x, float _y, uint32_t _button, bool _isDown ) = 0;
        virtual bool onGameMouseButtonEnd( uint32_t _touchId, float _x, float _y, uint32_t _button, bool _isDown ) = 0;
        virtual bool onGameMouseMove( uint32_t _touchId, float _x, float _y, float _dx, float _dy ) = 0;
        virtual bool onGameMouseWheel( uint32_t _button, float _x, float _y, int32_t _wheel ) = 0;
        virtual void onGameAppMouseEnter( float _x, float _y ) = 0;
        virtual void onGameAppMouseLeave() = 0;
        virtual void onGameTimingFactor( float _timingFactor ) = 0;
        virtual bool onGamePreparation( bool _debug ) = 0;
        virtual void onGameRun() = 0;
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
        virtual void onGameChangeSoundVolume( float _sound, float _music, float _voice ) = 0;
        virtual void onGameCursorMode( bool _mode ) = 0;
        virtual void onGameUser( const ConstString & _event, const MapWParams & _params ) = 0;
        virtual bool onGameClose() = 0;
        virtual void onGameOverFillrate( double _fillrate ) = 0;
        virtual void onGameFrameEnd() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    class GameServiceInterface
        : public ServiceInterface
        , public Eventable
        , public InputHandlerInterface
    {
        SERVICE_DECLARE( "GameService" )

    public:
        virtual void run() = 0;

    public:
        virtual void update() = 0;
        virtual void tick( const UpdateContext * _context ) = 0;

    public:
        virtual void render() = 0;

    public:
        virtual void setCursorMode( bool _mode ) = 0;

    public:
        virtual float getTimeFactor() const = 0;
        virtual void setTimeFactor( float _factor ) = 0;

    public:
        virtual bool getParam( const ConstString & _paramName, Char * _param ) const = 0;
        virtual bool hasParam( const ConstString & _paramName ) const = 0;

    public:
        virtual bool loadPersonality() = 0;

    public:
        virtual void initializeRenderResources() = 0;
        virtual void finalizeRenderResources() = 0;

    public:
        virtual void setFocus( bool _focus ) = 0;
        virtual void setFullscreen( const Resolution & _resolution, bool _fullscreen ) = 0;
        virtual void setFixedContentResolution( const Resolution & _resolution, bool _fixed ) = 0;
        virtual void setFixedDisplayResolution( const Resolution & _resolution, bool _fixed ) = 0;
        virtual void setRenderViewport( const Viewport & _viewport, const Resolution & _contentResolution ) = 0;
        virtual void setGameViewport( const Viewport & _viewport, float _aspect ) = 0;

        virtual bool close() = 0;

        virtual void userEvent( const ConstString & _event, const MapWParams & _params ) = 0;

        virtual void turnSound( bool _turn ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define GAME_SERVICE()\
    ((Mengine::GameServiceInterface*)SERVICE_GET(Mengine::GameServiceInterface))
//////////////////////////////////////////////////////////////////////////