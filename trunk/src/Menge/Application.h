#	pragma once

#	include "MengeExport.h"

//#	include "Loadable.h"

#	include "Core/Resolution.h"
#	include "Core/Viewport.h"

#	include "Interface/ApplicationInterface.h"

#	include "Core/ConstString.h"

#	include "Logger/Logger.h"

#	include "Math/vec4.h"

#	include <map>

namespace Menge
{
	class ServiceProvider;

    class LoaderServiceInterface;
    class ResourceServiceInterface;
    class AlphaChannelServiceInterface;
    class TextServiceInterface;
    class NodeServiceInterface;
    class ArrowServiceInterface;
    class SceneServiceInterface;
    class EntityServiceInterface;
    class MovieKeyFrameServiceInterface;
    class EventServiceInterface;

    class WatchdogInterface;

	class FileOutputStreamInterface;
	class Game;
	class ResourceManager;
	class CodecEngine;
	class TextManager;
	class NodeManager;
	class FileLogger;

	class Watchdog;

	class ResourceCursor;

	class PluginInterface;

	class UnicodeServiceInterface;

	class RenderTextureInterface;

	class Application 
		: public ApplicationInterface
		//, public Loadable
	{
	public:
		Application();
		~Application();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

    public:
        bool initialize() override;
        void finalize() override;

    public:
        bool setup( const String& _args, const ApplicationSettings & _setting ) override;
		
	public:
		const String & getBaseDir() const override;

	public:
		PlatformInterface * getPlatform() const override;

	public:
		bool getAllowFullscreenSwitchShortcut() const override;
        	
	public:
		bool createGame( const WString & _module, const ConstString & _language, const TVectorResourcePackDesc & _resourcePack, const TVectorResourcePackDesc & _languagePack ) override;
		bool initializeGame( const TMapWString & _params, const String & _scriptInitParams ) override;

	public:
		void setFullscreenMode( bool _fullscreen ) override;
		bool getFullscreenMode() const override;

    public:
        bool isValidWindowMode() const override;

	public:
		bool getVSync() const override;

	public:
		bool isFocus() const override;

	public:
		const String & getPlatformName() const override;

	protected:
		bool initializeNodeManager_();
		bool initializeLoaderEngine_();
		bool initializeResourceManager_();
		bool initializeArrowManager_();
		bool initializeSceneManager_();
		bool initializeEntityManager_();
		bool initializeTextManager_();
		bool initializeMovieKeyFrameManager_();
        bool initializeEventManager_();

	public:		
		void destroy() override;

	public:
		void setBaseDir( const String & _dir );

		bool createRenderWindow( WindowHandle _renderWindowHandle ) override;

		void screenshot( RenderTextureInterface * _renderTargetImage, const mt::vec4f & _rect );

		void quit();
		
	public:
		void calcWindowResolution( Resolution & _windowResolution ) const override;

		const Resolution & getCurrentResolution() const override;

		const Viewport & getRenderViewport() const override;

		const Resolution & getContentResolution() const override;
		void getGameViewport( float & _aspect, Viewport & _viewport ) const override;

	public:
		bool onRender() override;
		void onFlush() override;
		bool onUpdate() override;
		void onTick( float _timing ) override;
		void onFocus( bool _focus, const mt::vec2f & _point ) override;
		void onClose() override;
        
        void onTurnSound( bool _turn ) override;

		void onAppMouseEnter( const mt::vec2f & _point ) override;
		void onAppMouseLeave() override;		

		bool onKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown ) override;
		bool onMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, int _button, bool _isDown ) override;
		bool onMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y, int _whell ) override;
		
		void onPaint() override;

	public:
		void setParticlesEnabled( bool _enabled ) override;
		bool getParticlesEnabled() const override;

	public:
		void setInputMouseButtonEventBlock( bool _block ) override;
		bool getInputMouseButtonEventBlock() const override;

	public:
		void minimizeWindow() override;

		void setMouseBounded( bool _bounded ) override;
		bool getMouseBounded() const override;

		unsigned int getDebugMask() const override;

		const WString & getProjectTitle() const override;
		const String & getProjectCodename() const override;

	public:
		const Resolution & getWindowResolution() const;

    public:
        void setFixedContentResolution( bool _fixedContetResolution ) override; 
		bool getFixedContentResolution() const override;

	public:
		static const char* getVersionInfo();

		void enableDebug( bool _enable );
        		
		void updateNotification();
		void setVSync( bool _vsync ) override;
		void setCursorMode( bool _mode ) override;
		bool getCursorMode() const override;
		void setCursorIcon( const ConstString & _resourceName ) override;

		//void setAsScreensaver( bool _set );

		void showKeyboard() override;
		void hideKeyboard() override;

    public:
        bool findBestAspectViewport_( float _aspect, float & _bestAspect, Viewport & _viewport ) const; 

    public:
        WatchdogInterface * getWatchdog() const override;

	protected:		
		void calcRenderViewport_( const Resolution & _resolution, Viewport & _viewport );
        void invalidateWindow_();

	protected:
		PlatformInterface * m_platform;

		ConsoleInterface * m_console;

		typedef std::map<String, DynamicLibraryInterface*> TDynamicLibraries;
		TDynamicLibraries m_dynamicLibraries;

		Game * m_game;

		Resolution m_currentResolution;

		Viewport m_renderViewport;

		Viewport m_gameViewport;

        bool m_soundMute;
		bool m_particles;
		bool m_mouseBounded;
		bool m_focus;
		bool m_update;

		bool m_fullscreen;
		Resolution m_renderResolution;

		Resolution m_windowResolution;
		int m_bits;
		bool m_textureFiltering;
		int	m_FSAAType;
		int m_FSAAQuality;

		Resolution m_contentResolution;

        typedef std::map<float, Viewport> TMapAspectRatioViewports;
        TMapAspectRatioViewports m_aspectRatioViewports;

		bool m_fixedContentResolution;


		bool m_createRenderWindow;

		unsigned int m_debugMask;
		
		size_t m_countThreads;

		bool m_resetTiming;
		float m_phycisTiming;
		float m_maxTiming;
		
		ServiceProviderInterface * m_serviceProvider;

		LoaderServiceInterface * m_loaderService;
		ResourceServiceInterface * m_resourceService;
		TextServiceInterface* m_textService;
		NodeServiceInterface * m_nodeService;
		ArrowServiceInterface * m_arrowService;
		SceneServiceInterface * m_sceneService;
		EntityServiceInterface * m_entityService;
		MovieKeyFrameServiceInterface * m_movieKeyFrameService;		
        EventServiceInterface * m_eventService;

		WatchdogInterface * m_watchdog;

		void parseArguments_( const String& _arguments );

		String m_baseDir;
		
		ResourceCursor * m_cursorResource;
		
		String m_platformName;
		String m_projectCodename;
		
		typedef std::vector<PluginInterface *> TVectorPlugins;
		TVectorPlugins m_plugins;

		bool m_allowFullscreenSwitchShortcut;

		bool m_fullScreen;
		bool m_vsync;
		bool m_invalidateVsync;
		bool m_cursorMode;
		bool m_invalidateCursorMode;
		bool m_mouseEnter;

		bool m_inputMouseButtonEventBlock;
		bool m_developmentMode;

        bool m_windowModeCheck;
	};
}
