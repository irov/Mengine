#	pragma once

#	include "MengeExport.h"

#	include "Loadable.h"

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
	class TaskManager;
	class ArrowManager;
	class AccountManager;
	class SceneManager;
	class EntityManager;
	class ParamManager;

	class FileOutputStreamInterface;
	class Game;
	class ResourceManager;
	class AlphaChannelManager;
	class CodecEngine;
	class TextManager;
	class NodeManager;
	class FileLogger;
	

	class PluginInterface;

	class Consts;
	class Texture;

	class Application 
		: public Holder<Application>
		, public ApplicationInterface
		, public Loadable
	{
	public:
		Application();
		~Application();

	public:
		bool initialize( PlatformInterface* _platform, const String & _platformName, const String& _args ) override;
		bool loadConfig( const String& _configFile ) override;

		void setBaseDir( const String & _dir ) override;

	public:
		ServiceProviderInterface * getServiceProvider() const override;

	public:
		void setDesktopResolution( const Resolution& _resolution ) override;
		void setMaxClientAreaSize( size_t _maxWidth, size_t _maxHeight ) override;

	public:
		bool getAllowFullscreenSwitchShortcut() const override;
		bool getHasWindowPanel() const override;

	public:
		void setLanguagePack( const ConstString& _packName ) override;

	public:
		bool initGame( const String & _scriptInitParams ) override;
		bool loadGame() override;
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
		const String& getScreensaverName() const override;

	public:
		const String & getPlatformName() const;

	protected:
		bool initializeThreadEngine_();
		bool initializeFileEngine_();
		bool initializeLogEngine_();
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
		bool initializeParamManager_();
		bool initializeArrowManager_();
		bool initializeSceneManager_();
		bool initializeEntityManager_();
		bool initializeAlphaChannelManager_();
		bool initializeTextManager_();

	public:
		void finalize() override;
		void destroy() override;

	public:
		bool createRenderWindow( WindowHandle _renderWindowHandle, WindowHandle _inputWindowHandle ) override;

		void screenshot( Texture * _renderTargetImage, const mt::vec4f & _rect );

		void quit();

		const String & getPathGameFile() const;
				
		const String & getBaseDir() const;

	public:
		void loader( BinParser * _parser ) override;
		
	protected:
		void _loaded() override;

	protected:
		void loaderApplication_( BinParser * _parser );

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

		bool onMouseButtonEvent( const mt::vec2f & _point, int _button, bool _isDown ) override;
		bool onMouseMove( const mt::vec2f & _point, float _x, float _y, int _whell ) override;
		bool onKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown ) override;
		
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

		void showMessageBox( const String& _message, const String& _header, unsigned int _style );

		void ansiToUtf8( const String& _ansi, String & _utf8 );
		void utf8ToAnsi( const String& _utf8, String & _ansi );
		void utf8Count( const String& _utf8, size_t & _size );

		const String & getProjectTitle() const;
		
		static const char* getVersionInfo();

		void enableDebug( bool _enable );

		bool isDebugCRT() const;

		void setCursorPosition( const mt::vec2f & _point );

		
		const Resolution& getMaxClientAreaSize() const;		
		
		void updateNotification();
		void setVSync( bool _vsync );
		void setCursorMode( bool _mode );
		bool getCursorMode() const;
		void setCursorIcon(const String& _fileName);

		void pushKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown );
		void pushMouseButtonEvent( const mt::vec2f & _point, int _button, bool _isDown );
		void pushMouseMoveEvent( const mt::vec2f & _point, int _x, int _y, int _z );

		void setAsScreensaver( bool _set );

		void showKeyboard();
		void hideKeyboard();

	public:
		bool loadPlugin( const String& _pluginName, const TMapParam & _params );

	protected:		
		void unloadPlugins_();

		void calcRenderViewport_( Viewport & _viewport, const Resolution & _resolution );

	protected:
		PlatformInterface * m_platform;

		ScriptEngine * m_scriptEngine;

		ConsoleInterface * m_console;

		Consts * m_consts;

		typedef std::map<String, DynamicLibraryInterface*> TDynamicLibraries;
		TDynamicLibraries m_dynamicLibraries;

		Game * m_game;

		String m_gameDescription;

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

		LogEngine * m_logEngine;
		FileEngine * m_fileEngine;
		InputEngine * m_inputEngine;
		RenderEngine * m_renderEngine;
		SoundEngine * m_soundEngine;
		ParticleEngine * m_particleEngine;
		PhysicEngine2D * m_physicEngine2D;
		LoaderEngine *	m_loaderEngine;
		ThreadEngine* m_threadEngine;
		TaskManager* m_taskManager;
		ResourceManager* m_resourceManager;
		ParamManager * m_paramManager;
		AlphaChannelManager* m_alphaChannelManager;
		CodecEngine* m_codecEngine;
		TextManager* m_textManager;
		NodeManager* m_nodeManager;
		ArrowManager * m_arrowManager;
		SceneManager * m_sceneManager;
		EntityManager * m_entityManager;

		StringizeService * m_stringizeService;

		void parseArguments_( const String& _arguments );

		String m_baseDir;
		
		ConstString m_gamePackName;
		String m_gamePackPath;
		ConstString m_gamePackType;
		ConstString m_languagePackOverride;

		String m_platformName;
		
		typedef std::vector<PluginInterface *> TVectorPlugins;
		TVectorPlugins m_plugins;

		int m_alreadyRunningPolicy;
		bool m_allowFullscreenSwitchShortcut;

		bool m_vsync;
		bool m_invalidateVsync;
		bool m_cursorMode;
		bool m_invalidateCursorMode;

		bool m_inputMouseButtonEventBlock;
	};
}
