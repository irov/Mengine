#	pragma once

#	include "Kernel/Scriptable.h"
#	include "Kernel/Eventable.h"

#   include "Interface/GameInterface.h"
#	include "Interface/ApplicationInterface.h"
#   include "Interface/InputSystemInterface.h"

#	include "Account.h"

#	include "Core/Resolution.h"

#	include <map>

namespace Menge
{
	class AccountServiceListener;
	class AccountServiceInterface;

	class Player;
	class ResourcePak;

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
        bool initialize( const TMapParams & _params ) override;
        void finalize() override;

    public:
        void setBaseDir( const FilePath & _baseDir ) override;
        void setDevelopmentMode( bool _developmentMode ) override;
        void setPlatformName( const ConstString & _platformName ) override;

    public:
        PlayerServiceInterface * getPlayer() const override;

	public:
		bool update() override;
		void tick( float _timing ) override;

    public:
		bool render() override;

    public:
        bool run( const String& _scriptInitParams ) override;

    public:
		void initializeRenderResources() override;
		void finalizeRenderResources() override;

		bool loadPersonality( const ConstString & _module ) override;
		
		bool loadConfigPaks( const TVectorResourcePackDesc & _resourcePack, const TVectorResourcePackDesc & _languagePack ) override;
		bool applyConfigPaks() override;

		void setLanguagePack( const ConstString& _packName ) override;
        const ConstString & getLanguagePack() const override;

	protected:
		bool loadLocalePaksByName_( const ConstString & _locale, const ConstString & _platform );
			
	public:
		void setCursorMode( bool _mode ) override;

	public:
		bool handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown ) override;
		bool handleMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;		
		bool handleMouseButtonEventBegin( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventEnd( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y, int _whell ) override;
		
		void onAppMouseLeave() override;
		void onAppMouseEnter( const mt::vec2f & _point ) override;

		void onFocus( bool _focus ) override;
		void onFullscreen( const Resolution & _resolution, bool _fullscreen ) override;
        void onFixedContentResolution( const Resolution & _resolution, bool _fixed ) override;
        void onRenderViewport( const Viewport & _viewport, const Resolution & _contentResolution ) override;
		bool onClose() override;

        void onUserEvent( const ConstString & _event, const TMapParams & _params ) override;

		void onTurnSound( bool _turn ) override;

	public:
		const WString & getParam( const ConstString & _paramName ) override;
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

		AccountServiceListener * m_accountLister; 
		AccountServiceInterface * m_accountService;

		bool m_developmentMode;
        
		Arrow * m_defaultArrow;
		
		ConstString m_currentPakName;
		String m_currentResourcePath;

		bool m_hasWindowPanel;
		float m_timingFactor;

		FilePath m_baseDir;
		FilePath m_iconPath;

		typedef std::vector<ResourcePak *> TVectorResourcePak;
		TVectorResourcePak m_paks;
		TVectorResourcePak m_resourcePaks;
		TVectorResourcePak m_languagePaks;

		ConstString m_languagePak;
		ConstString m_platformName;
				
		TMapParams m_params;

	protected:
		void initPredefinedResources_();
		void removePredefinedResources_();

    protected:
        void setEmbed( PyObject * _embed );
        PyObject * getEmbed() const;

    protected:
        PyObject * m_embed;
	};	
}
