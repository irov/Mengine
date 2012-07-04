#	pragma once

#	include "MengeExport.h"

//#	include "Loadable.h"

#	include "Core/Resolution.h"
#	include "Core/Viewport.h"

#	include "Interface/ApplicationInterface.h"

#	include "Core/ConstString.h"

#	include "Core/Holder.h"
#	include "LogEngine.h"

#	include "Math/vec4.h"

#	include <map>

namespace Menge
{
	class ServiceProvider;

	class StringizeService;

	class LogEngine;
	class ScriptEngine;
	class LogEngine;
	class FileEngine;
	class InputEngine;
	class RenderEngine;
	class SoundEngine;
	class ParticleEngine;
	class PhysicEngine2D;
	class PhysicEngine;
	class LoaderEngine;
	class ThreadEngine;
	class ThreadTaskManager;
	class ArrowManager;
	class AccountManager;
	class SceneManager;
	class EntityManager;
	class ConverterEngine;
	class MovieKeyFrameManager;

	class FileOutputStreamInterface;
	class Game;
	class ResourceManager;
	class AlphaChannelManager;
	class CodecEngine;
	class TextManager;
	class NodeManager;
	class FileLogger;

	class Watchdog;

	class PluginInterface;

	class UnicodeServiceInterface;

	class Consts;
	class RenderTextureInterface;

	class Application 
		: public Holder<Application>
		, public ApplicationInterface
		//, public Loadable
	{
	public:
		Application();
		~Application();

	public:
		bool initialize( PlatformInterface* _platform, const String & _platformName, const String& _args, const WString & _baseDir, const WString & _settingFile ) override;
		bool loadConfig( const WString& _iniFile ) override;

		void setBaseDir( const WString & _dir ) override;
		const WString & getBaseDir() const override;

	public:
		PlatformInterface * getPlatform() const override;
		ServiceProviderInterface * getServiceProvider() const override;

	public:
		void setDesktopResolution( const Resolution& _resolution ) override;
		void setMaxClientAreaSize( size_t _maxWidth, size_t _maxHeight ) override;

	public:
		bool getAllowFullscreenSwitchShortcut() const override;
		bool getHasWindowPanel() const override;

	public:
		void setLanguagePackOverride( const ConstString& _packName ) override;

	public:		
		bool createGame() override;
		bool loadGameResource() override;
		bool initializeGame( const String & _scriptInitParams ) override;

	public:
		bool loadPlugins() override;
		bool loadPersonality() override;

	public:
		int getAlreadyRunningPolicy() const override;

	public:
		void setFullscreenMode( bool _fullscreen ) override;
		bool getFullscreenMode() const override;

	public:
		const Resolution& getResolution() const override;
		bool getVSync() const override;

	public:
		bool isFocus() const override;

	public:
		//bool initialize( const String& _applicationFile, const String& _args );
		const WString& getScreensaverName() const override;

	public:
		const String & getPlatformName() const;

	protected:
		bool initializeThreadEngine_();
		bool initializeFileEngine_();
		bool initializeLogEngine_();
		bool initializeUnicodeEngine_();
		bool initializeParticleEngine_();
		bool initializePhysicEngine2D_();
		bool initializeRenderEngine_();
		bool initializeSoundEngine_();
		bool initializeTaskManager_();
		bool initializeNodeManager_();
		bool initializeLoaderEngine_();
		bool initializeScriptEngine_();
		bool initializeCodecEngine_();
		bool initializeResourceManager_();
		bool initializeArrowManager_();
		bool initializeSceneManager_();
		bool initializeEntityManager_();
		bool initializeAlphaChannelManager_();
		bool initializeTextManager_();
		bool initializeConverterEngine_();
		bool initializeMovieKeyFrameManager_();
		bool initializeInputEngine_();

	public:
		void finalize() override;
		void destroy() override;

	public:
		bool createRenderWindow( WindowHandle _renderWindowHandle, WindowHandle _inputWindowHandle ) override;

		void screenshot( RenderTextureInterface * _renderTargetImage, const mt::vec4f & _rect );

		void quit();

		const WString & getPathGameFile() const;
		
	//public:
	//	void loader( BinParser * _parser ) override;
	//	
	//protected:
	//	void _loaded() override;

	//protected:
	//	void loaderApplication_( BinParser * _parser );

	public:
		const Resolution & getCurrentResolution() const;
		const Resolution & getDesktopResolution() const;

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

		bool onKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown );
		bool onMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, int _button, bool _isDown );
		bool onMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y, int _whell );		
		
		void onPaint() override;

	public:
		void setParticlesEnabled( bool _enabled );
		bool getParticlesEnabled() const;

		void setSoundEnabled( bool _enabled );
		bool getSoundEnabled()	const;

	public:
		void setInputMouseButtonEventBlock( bool _block );
		bool getInputMouseButtonEventBlock() const;

	public:
		void minimizeWindow();

		void setMouseBounded( bool _bounded );
		bool getMouseBounded() const;

		unsigned int getDebugMask() const;

		void showMessageBox( const WString& _message, const WString& _header, unsigned int _style );

		void ansiToUtf8( const String& _ansi, String & _utf8 );
		void utf8ToAnsi( const String& _utf8, String & _ansi );
		void utf8Count( const String& _utf8, size_t & _size );

		const WString & getProjectTitle() const;
		const ConstString & getProjectName() const;
		
		static const char* getVersionInfo();

		void enableDebug( bool _enable );

		bool isDebugCRT() const;
		
		const Resolution& getMaxClientAreaSize() const;		
		
		void updateNotification();
		void setVSync( bool _vsync );
		void setCursorMode( bool _mode );
		bool getCursorMode() const;
		void setCursorIcon(const WString& _fileName);

		void setAsScreensaver( bool _set );

		void showKeyboard();
		void hideKeyboard();

	public:
		bool loadPlugin( const WString& _pluginName );

	protected:		
		void unloadPlugins_();

		void calcRenderViewport_( const Resolution & _resolution, Viewport & _viewport );

	protected:
		PlatformInterface * m_platform;

		ScriptEngine * m_scriptEngine;

		ConsoleInterface * m_console;

		Consts * m_consts;

		typedef std::map<WString, DynamicLibraryInterface*> TDynamicLibraries;
		TDynamicLibraries m_dynamicLibraries;

		Game * m_game;

		WString m_gameDescription;

		Resolution m_currentResolution;
		Resolution m_desktopResolution;
		Resolution m_maxClientAreaSize;

		Viewport m_gameViewport;

		bool m_particles;
		bool m_sound;
		bool m_mouseBounded;
		bool m_focus;
		bool m_update;

		bool m_fullscreen;
		Resolution m_renderResolution;

		bool m_createRenderWindow;

		unsigned int m_debugMask;

		bool m_debugCRT;
		
		size_t m_countThreads;

		bool m_resetTiming;
		float m_phycisTiming;
		float m_maxTiming;
		
		ServiceProvider * m_serviceProvider;

		UnicodeServiceInterface * m_unicodeService;

		LogEngine * m_logEngine;
		FileEngine * m_fileEngine;
		InputEngine * m_inputEngine;
		RenderEngine * m_renderEngine;
		SoundEngine * m_soundEngine;
		ParticleEngine * m_particleEngine;
		PhysicEngine2D * m_physicEngine2D;
		LoaderEngine *	m_loaderEngine;
		ThreadEngine* m_threadEngine;
		ThreadTaskManager* m_taskManager;
		ResourceManager* m_resourceManager;
		AlphaChannelManager* m_alphaChannelManager;
		CodecEngine* m_codecEngine;
		TextManager* m_textManager;
		NodeManager* m_nodeManager;
		ArrowManager * m_arrowManager;
		SceneManager * m_sceneManager;
		EntityManager * m_entityManager;
		ConverterEngine * m_converterEngine;
		MovieKeyFrameManager * m_movieKeyFrameManager;

		StringizeService * m_stringizeService;

		Watchdog * m_watchdog;

		void parseArguments_( const String& _arguments );

		WString m_applicationPath;
		WString m_baseDir;
		
		WString m_cursorFileName;
		
		ConstString m_gamePackName;
		WString m_gamePackPath;
		ConstString m_gamePackType;
		ConstString m_languagePackOverride;

		TVectorWString m_initPlugins;

		String m_platformName;
		
		typedef std::vector<PluginInterface *> TVectorPlugins;
		TVectorPlugins m_plugins;

		int m_alreadyRunningPolicy;
		bool m_allowFullscreenSwitchShortcut;

		bool m_vsync;
		bool m_invalidateVsync;
		bool m_cursorMode;
		bool m_invalidateCursorMode;
		bool m_mouseEnter;

		bool m_inputMouseButtonEventBlock;
		bool m_developmentMode;
	};
}
