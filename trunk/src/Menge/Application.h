#	pragma once

#	include "Interface/ApplicationInterface.h"

#   include "Interface/PrototypeManagerInterface.h"
#	include "Interface/TextInterface.h"

#   include "Interface/InputSystemInterface.h"
#   include "Interface/RenderSystemInterface.h"
#	include "Interface/SoundSystemInterface.h"

#   include "Interface/ParticleSystemInterface.h"
#   include "Interface/ScriptSystemInterface.h"
#   include "Interface/EventInterface.h"
#   include "Interface/PhysicSystemInterface.h"
#   include "Interface/ConfigInterface.h"

#   include "Interface/StringizeInterface.h"

#	include "Interface/NotificationServiceInterface.h"
#	include "Interface/NotificatorInterface.h"

#   include "Interface/MousePickerSystemInterface.h"

#	include "Interface/LoaderInterface.h"
#	include "Interface/NodeInterface.h"
#	include "Interface/ThreadSystemInterface.h"
#	include "Interface/CodecInterface.h"

#   include "Interface/ConverterInterface.h"

#   include "Interface/ResourceInterface.h"
#   include "Interface/AlphaChannelInterface.h"
#   include "Interface/AccountInterface.h"
#   include "Interface/ProfilerInterface.h"

#   include "Interface/GameInterface.h"
#   include "Interface/WatchdogInterface.h"
#   include "Interface/GraveyardInterface.h"

#   include "Consts.h"

#	include "Core/Resolution.h"
#	include "Core/Viewport.h"
#	include "Core/ConstString.h"

#	include "Logger/Logger.h"

#	include "Math/vec4.h"

#	include <stdex/stl_map.h>

namespace Menge
{
	class ResourceCursor;

	class Application 
		: public ApplicationInterface
	{
	public:
		Application();
		~Application();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

    public:
        bool initialize( const String & _args ) override;
        void finalize() override;
		
	protected:
		bool loadResourcePacks_( const ConstString & _fileGroup, const FilePath & _resourceIni );

	public:
		bool getAllowFullscreenSwitchShortcut() const override;
        	
	public:
		void setDefaultWindowDescription( const Resolution & _resolution, uint32_t _bits, bool _fullscreen, bool _vsync ) override;

	public:
		bool createGame( const ConstString & _module, const ConstString & _language, const ConstString & _resourcesIniGroup, const FilePath & _resourcesIniPath ) override;
		bool initializeGame( const String & _scriptInitParams ) override;
		void finalizeGame() override;

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
		const ConstString & getPlatformName() const override;

	protected:
		bool initializeNodeManager_();
		bool initializeLoaderEngine_();
		bool initializeResourceManager_();
		bool initializeSceneManager_();
		bool initializeTextManager_();
        bool initializePrototypeManager_();
        bool initializeWatchdog_();
        bool initializeProfiler_();
		bool initializeGraveyard_();

	public:
		bool createRenderWindow( WindowHandle _renderWindowHandle ) override;

		//void screenshot( const RenderTextureInterfacePtr & _renderTargetImage, const mt::vec4f & _rect );

		void quit();
		
	public:
		void calcWindowResolution( Resolution & _windowResolution ) const override;

		const Resolution & getCurrentResolution() const override;

		const Viewport & getRenderViewport() const override;

		const Resolution & getContentResolution() const override;
		void getGameViewport( float & _aspect, Viewport & _viewport ) const override;

	public:
		bool render() override;
		void flush() override;
		bool beginUpdate() override;
		void tick( float _timing ) override;
		void endUpdate() override;
		void setFocus( bool _focus, const mt::vec2f & _point ) override;
		void close() override;
        
        void turnSound( bool _turn ) override;

		void mouseEnter( const mt::vec2f & _point ) override;
		void mouseLeave() override;		

		bool keyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown, bool _repeating ) override;
		bool mouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, int _button, bool _isDown ) override;		
		bool mouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y ) override;
		bool mouseWheel( unsigned int _touchId, const mt::vec2f & _point, int _wheel ) override;
		void mousePosition( unsigned int _touchId, const mt::vec2f & _point ) override;
		
		
		void paint() override;

        bool userEvent( const ConstString & _event, const TMapParams & _params ) override;

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

	public:
		const WString & getCompanyName() const override;
		const WString & getProjectName() const override;

	public:
		const ConstString & getProjectTitle() const override;
		const ConstString & getProjectCodename() const override;
		uint32_t getProjectVersion() const override;

	public:
		const Resolution & getWindowResolution() const;

    public:
        void setFixedContentResolution( bool _fixedContetResolution ) override; 
		bool getFixedContentResolution() const override;

	public:
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

	protected:		
		void calcRenderViewport_( const Resolution & _resolution, Viewport & _viewport );
        void invalidateWindow_();

	protected:
		ServiceProviderInterface * m_serviceProvider;

		ConsoleInterface * m_console;

		typedef stdex::map<String, DynamicLibraryInterface *> TDynamicLibraries;
		TDynamicLibraries m_dynamicLibraries;

		GameServiceInterface * m_game;

		Resolution m_currentResolution;

		Viewport m_renderViewport;

		Viewport m_gameViewport;

		bool m_particles;
		bool m_mouseBounded;
		bool m_focus;
		bool m_update;

		Resolution m_windowResolution;
		uint32_t m_bits;
		bool m_fullscreen;
		bool m_vsync;

		Resolution m_renderResolution;

		bool m_textureFiltering;
		int	m_FSAAType;
		int m_FSAAQuality;

		Resolution m_contentResolution;

        typedef stdex::map<float, Viewport> TMapAspectRatioViewports;
        TMapAspectRatioViewports m_aspectRatioViewports;

		bool m_fixedContentResolution;
		bool m_fixedDisplayResolution;
		bool m_createRenderWindow;

		unsigned int m_debugMask;
		
		uint32_t m_countThreads;

		bool m_resetTiming;
		float m_phycisTiming;
		float m_maxTiming;

		LoaderServiceInterface * m_loaderService;
		ResourceServiceInterface * m_resourceService;        
		TextServiceInterface* m_textService;
		NodeServiceInterface * m_nodeService;		
        PrototypeServiceInterface * m_prototypeService;
		GraveyardInterface * m_graveyard;
        Consts * m_consts;

		WatchdogInterface * m_watchdog;
        ProfilerServiceInterface * m_profiler;

		void parseArguments_( const String& _arguments );
		
		ResourceCursor * m_cursorResource;

		WString m_companyName;
		WString m_projectName;

		ConstString m_platformName;
		ConstString m_projectCodename;
		uint32_t m_projectVersion;

		bool m_allowFullscreenSwitchShortcut;

		
		bool m_invalidateVsync;
		bool m_cursorMode;
		bool m_invalidateCursorMode;
		bool m_mouseEnter;

		bool m_inputMouseButtonEventBlock;
		bool m_developmentMode;
		bool m_masterMode;
        bool m_nofullscreenMode;

        bool m_windowModeCheck;
        bool m_resourceCheck;
		bool m_resourceCheckCritical;
	};
}
