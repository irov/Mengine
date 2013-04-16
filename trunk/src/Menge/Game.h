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

	class LightSystem;

	class Game
		: public GameServiceInterface
		, public InputSystemHandler
		, public Eventable
	{
	public:
		Game( const String & _baseDir, bool _developmentMode, const String & _platformName );
		~Game();

    public:
        Player * getPlayer() const;

	public:
		bool update();
		void tick( float _timing );

    public:
		bool render() override;

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool initialize( const TMapWString & _params );
		void finalize();

    public:
        bool run( const String& _scriptInitParams );

    public:
		void initializeRenderResources();
		void finalizeRenderResources();

		bool loadPersonality( const ConstString & _module );
		
		bool loadConfigPaks( const TVectorResourcePackDesc & _resourcePack, const TVectorResourcePackDesc & _languagePack );
		bool applyConfigPaks();

		void setLanguagePack( const ConstString& _packName );
        const ConstString & getLanguagePack() const override;

	protected:
		bool loadLocalePaksByName_( const ConstString & _locale, const String & _platform );
			
	public:
		void setCursorMode( bool _mode );

	public:
		bool handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown ) override;
		bool handleMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;		
		bool handleMouseButtonEventBegin( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventEnd( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y, int _whell ) override;
		
		void onAppMouseLeave();
		void onAppMouseEnter( const mt::vec2f & _point );

		void onFocus( bool _focus );
		void onFullscreen( const Resolution & _resolution, bool _fullscreen );
        void onFixedContentResolution( const Resolution & _resolution, bool _fixed );
		bool onClose();

		void onTurnSound( bool _turn );

	public:
		void addMouseMoveHandler( PyObject * _cb ) override;
		bool removeMouseMoveHandler( PyObject * _cb ) override;

		void addMouseButtonHandler( PyObject * _cb ) override;
		bool removeMouseButtonHandler( PyObject * _cb ) override;

	public:
		const WString & getParam( const WString & _paramName ) override;
		bool hasParam( const WString & _paramName ) const override;

		bool getHasWindowPanel() const;

	public:
		float getTimingFactor() const override;
		void setTimingFactor( float _timingFactor ) override;

	public:
		void destroyArrow();

	private:
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

		String m_baseDir;
		FilePath m_iconPath;

		typedef std::vector<ResourcePak *> TVectorResourcePak;
		TVectorResourcePak m_paks;
		TVectorResourcePak m_resourcePaks;
		TVectorResourcePak m_languagePaks;

		ConstString m_languagePak;
		String m_platformName;
				
		bool m_personalityHasOnClose;

		typedef std::vector<PyObject *> TVectorHandlers;
		TVectorHandlers m_handlersMouseMove;
		TVectorHandlers m_handlersMouseButton;
		TMapWString m_params;

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
