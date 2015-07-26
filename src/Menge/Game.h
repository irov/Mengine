#	pragma once

#	include "Kernel/Scriptable.h"
#	include "Kernel/Eventable.h"

#   include "Interface/GameInterface.h"
#	include "Interface/ApplicationInterface.h"
#   include "Interface/InputSystemInterface.h"
#	include "Interface/AccountInterface.h"
#	include "Interface/SoundSystemInterface.h"

#	include "Account.h"
#	include "Pack.h"

#	include "Core/Resolution.h"

#	include "Factory/FactoryStore.h"

namespace Menge
{
	class Game;

	typedef stdex::vector<PackPtr> TVectorResourcePack;

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
		: public GameServiceInterface
		, public Eventable
	{
	public:
		Game();
		~Game();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

    public:
        bool initialize( const FilePath & _accountPath, uint32_t _projectVersion, const TMapParams & _params, const String & _scriptInitParams ) override;
        void finalize() override;

	public:
		void run() override;

	public:
		void tick( float _timing ) override;

    public:
		void render() override;

    public:
		void initializeRenderResources() override;
		void finalizeRenderResources() override;

		bool loadPersonality( const ConstString & _module ) override;
		
	public:
		bool addResourcePack( const ResourcePackDesc & _desc ) override;
		
	public:
		bool applyConfigPacks() override;

		void setLanguagePack( const ConstString& _packName ) override;
        const ConstString & getLanguagePack() const override;

	protected:
		bool loadLocalePacksByName_( TVectorResourcePack & _packs, const ConstString & _locale, const ConstString & _platform );
			
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

        void userEvent( const ConstString & _event, const TMapParams & _params ) override;

		void turnSound( bool _turn ) override;

	public:
		const WString & getParam( const ConstString & _paramName ) const override;
		bool hasParam( const ConstString & _paramName ) const override;

	public:
		bool addData( const ConstString & _name, const DataDesc & _desc ) override;
		bool hasData( const ConstString & _name ) const override;

		CacheBufferID loadData( const ConstString & _name, const void ** _data, size_t & _size ) const override;
		bool writeData( const ConstString & _name, const void * _data, size_t _size ) const override;

	public:
		float getTimingFactor() const override;
		void setTimingFactor( float _timingFactor ) override;

	public:
		void destroyArrow();

    protected:
        ServiceProviderInterface * m_serviceProvider;

		Player * m_player;

		GameAccountProvider * m_accountProvider; 
		GameSoundVolumeProvider * m_soundVolumeProvider;

		AccountServiceInterface * m_accountService;

		Arrow * m_defaultArrow;
		
		ConstString m_currentPackName;
		String m_currentResourcePath;

		float m_timingFactor;

		FilePath m_iconPath;
					
		TVectorResourcePack m_resourcePacks;

		typedef FactoryPoolStore<Pack, 8> TFactoryPack;
		TFactoryPack m_factoryPack;

		ConstString m_languagePack;
				
		TMapParams m_params;

		ArchivatorInterfacePtr m_archivator;
		
		typedef stdex::map<ConstString, DataDesc> TMapDatas;
		TMapDatas m_datas;

    protected:
		void registerEventMethods_( const pybind::object & _embed );
	};	
}
