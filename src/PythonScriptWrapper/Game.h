#	pragma once

#	include "Kernel/Scriptable.h"
#	include "Kernel/Eventable.h"

#   include "Interface/GameInterface.h"
#	include "Interface/ApplicationInterface.h"
#   include "Interface/InputSystemInterface.h"
#	include "Interface/AccountInterface.h"
#	include "Interface/SoundSystemInterface.h"
#	include "Interface/ScriptSystemInterface.h"

#   include "Core/ServiceBase.h"

#	include "Core/Resolution.h"
#   include "Core/String.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    class GameAccountProvider;
    class GameSoundVolumeProvider;
    //////////////////////////////////////////////////////////////////////////
    enum GameEventFlags
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
        EVENT_GAME_LOAD_ACCOUNTS,
        EVENT_GAME_CREATE_ACCOUNT,
        EVENT_GAME_DELETE_ACCOUNT,
        EVENT_GAME_SELECT_ACCOUNT,
        EVENT_GAME_UNSELECT_ACCOUNT,
        EVENT_GAME_CHANGE_SOUND_VOLUME,
        EVENT_GAME_CURSOR_MODE,
        EVENT_GAME_USER,
        EVENT_GAME_CLOSE
    };
    //////////////////////////////////////////////////////////////////////////
    class GameEventReceiver
        : public EventReceiver
    {
    public:
        virtual void onGameFullscreen( bool _fullscreen ) = 0;
        virtual void onGameFixedContentResolution( bool _fixed ) = 0;
        virtual void onGameFixedDisplayResolution( bool _fixed ) = 0;
        virtual void onGameRenderViewport( const Viewport & _viewport, const Resolution & _contentResolution ) = 0;
        virtual void onGameViewport( const Viewport & _viewport, float _aspect ) = 0;
        virtual bool onGameKey( KeyCode _code, float _x, float _y, bool _isDown, bool _isRepeat ) = 0;
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
        virtual void onGameLoadAccounts() = 0;
        virtual void onGameCreateAccount( const WString & _accountID ) = 0;
        virtual void onGameDeleteAccount( const WString & _accountID ) = 0;
        virtual void onGameSelectAccount( const WString & _accountID ) = 0;
        virtual void onGameUselectAccount( const WString & _accountID ) = 0;
        virtual void onGameChangeSoundVolume( float _sound, float _music, float _voice ) = 0;
        virtual void onGameCursorMode( bool _mode ) = 0;
        virtual void onGameUser( const ConstString & _event, const TMapWParams & _params ) = 0;
        virtual bool onGameClose() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
	class Game
		: public ServiceBase<GameServiceInterface>
		, public Eventable
	{
        EVENT_RECEIVER( GameEventReceiver );

	public:
		Game();
		~Game();

    public:
        bool _initialize() override;
        void _finalize() override;

	public:
		void run() override;

	public:
		void update() override;
		void tick( float _time, float _timing ) override;

    public:
		void render() override;

    public:
		void initializeRenderResources() override;
		void finalizeRenderResources() override;

	public:
		bool loadPersonality() override;
		
	public:
		void setCursorMode( bool _mode ) override;

	public:
		bool handleKeyEvent( const InputKeyEvent & _event ) override;
		bool handleTextEvent( const InputTextEvent & _event ) override;

	public:
		bool handleMouseButtonEvent( const InputMouseButtonEvent & _event ) override;
		bool handleMouseButtonEventBegin( const InputMouseButtonEvent & _event ) override;
		bool handleMouseButtonEventEnd( const InputMouseButtonEvent & _event ) override;
		bool handleMouseMove( const InputMouseMoveEvent & _event ) override;
		bool handleMouseWheel( const InputMouseWheelEvent & _event ) override;
		
	public:
		void mousePosition( const InputMousePositionEvent & _event ) override;
		void mouseEnter( const InputMousePositionEvent & _event ) override;
		void mouseLeave( const InputMousePositionEvent & _event ) override;

	public:
		void setFocus( bool _focus ) override;
		void setFullscreen( const Resolution & _resolution, bool _fullscreen ) override;
        void setFixedContentResolution( const Resolution & _resolution, bool _fixed ) override;
		void setFixedDisplayResolution( const Resolution & _resolution, bool _fixed ) override;
        void setRenderViewport( const Viewport & _viewport, const Resolution & _contentResolution ) override;
		void setGameViewport( const Viewport & _viewport, float _aspect ) override;

		bool close() override;

		void userEvent( const ConstString & _id, const TMapWParams & _params ) override;

		void turnSound( bool _turn ) override;

	public:
		WString getParam( const ConstString & _paramName ) const override;
		bool hasParam( const ConstString & _paramName ) const override;

	public:
		float getTimingFactor() const override;
		void setTimingFactor( float _timingFactor ) override;

	public:
		void destroyArrow();

    protected:
		Arrow * m_defaultArrow;
		
		ConstString m_currentPackName;
		String m_currentResourcePath;

		float m_timingFactor;

		FilePath m_iconPath;
				
		TMapWParams m_params;

		struct UserEvent
		{
			ConstString id;
			TMapWParams params;
		};

		typedef std::vector<UserEvent> TVectorUserEvents;
		TVectorUserEvents m_userEventsAdd;
		TVectorUserEvents m_userEvents;
		
    protected:
		void registerEventMethods_( const ScriptModuleInterfacePtr & _module );
	};	
}
