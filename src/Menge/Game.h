#	pragma once

#	include "Kernel/Scriptable.h"
#	include "Kernel/Eventable.h"

#   include "Interface/GameInterface.h"
#	include "Interface/ApplicationInterface.h"
#   include "Interface/InputSystemInterface.h"
#	include "Interface/AccountInterface.h"
#	include "Interface/SoundSystemInterface.h"
#	include "Interface/ScriptSystemInterface.h"

#	include "Account.h"

#	include "Core/Resolution.h"

#	include "Factory/FactoryStore.h"

namespace Menge
{
	class Game;

	class GameAccountProvider
		: public AccountProviderInterface		
	{
	public:
		GameAccountProvider( ServiceProviderInterface * _serviceProvider, Game * _game );

	protected:
		void onCreateAccount( const WString & _accountID ) override;
		void onDeleteAccount( const WString & _accountID ) override;
		void onSelectAccount( const WString & _accountID ) override;
		void onUnselectAccount( const WString & _accountID ) override;

	protected:
		ServiceProviderInterface * m_serviceProvider;
		Game * m_game;
	};

	class GameSoundVolumeProvider
		: public SoundVolumeProviderInterface
	{
	public:
		GameSoundVolumeProvider( ServiceProviderInterface * _serviceProvider, Game * _game );

	protected:
		void onSoundChangeVolume( float _sound, float _music, float _voice ) override;

	protected:
		ServiceProviderInterface * m_serviceProvider;
		Game * m_game;
	};
	
	class AccountServiceInterface;

	class Player;

	class Node;
	class Scene;
	class Arrow;
    
	class Game
		: public ServiceBase<GameServiceInterface>
		, public Eventable
	{
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
        void setRenderViewport( const Viewport & _viewport, const Resolution & _contentResolution ) override;
		void setGameViewport( const Viewport & _viewport, float _aspect ) override;

		bool close() override;

		void userEvent( const ConstString & _id, const TMapParams & _params ) override;

		void turnSound( bool _turn ) override;

	public:
		const WString & getParam( const ConstString & _paramName ) const override;
		bool hasParam( const ConstString & _paramName ) const override;

	public:
		float getTimingFactor() const override;
		void setTimingFactor( float _timingFactor ) override;

	public:
		void destroyArrow();

    protected:
		Player * m_player;

		GameAccountProvider * m_accountProvider; 
		GameSoundVolumeProvider * m_soundVolumeProvider;

		Arrow * m_defaultArrow;
		
		ConstString m_currentPackName;
		String m_currentResourcePath;

		float m_timingFactor;

		FilePath m_iconPath;
				
		TMapParams m_params;

		ArchivatorInterfacePtr m_archivator;

		struct UserEvent
		{
			ConstString id;
			TMapParams params;
		};

		typedef std::vector<UserEvent> TVectorUserEvents;
		TVectorUserEvents m_eventsAdd;
		TVectorUserEvents m_events;
		
    protected:
		void registerEventMethods_( const ScriptModuleInterfacePtr & _module );
	};	
}
