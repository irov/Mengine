#	pragma once

#	include "Kernel/Scriptable.h"
#	include "Kernel/Eventable.h"

#   include "Interface/GameInterface.h"
#	include "Interface/ApplicationInterface.h"
#   include "Interface/InputSystemInterface.h"
#	include "Interface/AccountInterface.h"
#	include "Interface/SoundSystemInterface.h"

#	include "Account.h"
#	include "Pak.h"

#	include "Core/Resolution.h"

#	include "Factory/FactoryStore.h"

namespace Menge
{
	class Game;

	typedef stdex::vector<PakPtr> TVectorResourcePak;

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
        void setDevelopmentMode( bool _developmentMode ) override;

    public:
        PlayerServiceInterface * getPlayer() const override;

	public:
		bool beginUpdate() override;
		void tick( float _timing ) override;
		void endUpdate() override;

    public:
		void render() override;

    public:
		void initializeRenderResources() override;
		void finalizeRenderResources() override;

		bool loadPersonality( const ConstString & _module ) override;
		
		void addResourcePak( const ResourcePackDesc & _desc ) override;
		void addLanguagePak( const ResourcePackDesc & _desc ) override;

	public:
		bool loadResourcePak( const ResourcePackDesc & _desc ) override;

	public:
		bool applyConfigPaks() override;

		void setLanguagePack( const ConstString& _packName ) override;
        const ConstString & getLanguagePack() const override;

	protected:
		bool loadLocalePaksByName_( TVectorResourcePak & _paks, const ConstString & _locale, const ConstString & _platform );
			
	public:
		void setCursorMode( bool _mode ) override;

	public:
		bool handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown, bool _repeating ) override;
		bool handleMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;		
		bool handleMouseButtonEventBegin( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventEnd( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y ) override;
		bool handleMouseWheel( unsigned int _touchId, const mt::vec2f & _point, int _wheel ) override;
		
		void mouseLeave() override;
		void mouseEnter( const mt::vec2f & _point ) override;
		void mousePosition( const mt::vec2f & _point ) override;

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

		bool getHasWindowPanel() const;

	public:
		float getTimingFactor() const override;
		void setTimingFactor( float _timingFactor ) override;

	public:
		void destroyArrow();

    protected:
        ServiceProviderInterface * m_serviceProvider;

		Player* m_player;

		GameAccountProvider * m_accountProvider; 
		GameSoundVolumeProvider * m_soundVolumeProvider;

		AccountServiceInterface * m_accountService;

		bool m_developmentMode;
        
		Arrow * m_defaultArrow;
		
		ConstString m_currentPakName;
		String m_currentResourcePath;

		bool m_hasWindowPanel;
		float m_timingFactor;

		FilePath m_iconPath;
					
		TVectorResourcePak m_resourcePaks;
		TVectorResourcePak m_languagePaks;

		typedef FactoryPoolStore<Pak, 8> TFactoryPak;
		TFactoryPak m_factoryPak;

		ConstString m_languagePak;
				
		TMapParams m_params;

    protected:
        void setEmbed( PyObject * _embed );
        PyObject * getEmbed() const;

    protected:
        PyObject * m_embed;
	};	
}
