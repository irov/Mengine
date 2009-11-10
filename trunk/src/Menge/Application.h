#	pragma once

#	include "MengeExport.h"

#	include "Resolution.h"

#	include "Interface/ApplicationInterface.h"

#	include "Holder.h"
#	include "LogEngine.h"

// already running policy
#	define ARP_NONE			0
#	define ARP_SETFOCUS		1
#	define ARP_SHOWMESSAGE	2

class XmlElement;

namespace Menge
{
	class ScriptEngine;
	class LogEngine;
	class FileEngine;
	class InputEngine;
	class RenderEngine;
	class SoundEngine;
	class ParticleEngine;
	class PhysicEngine2D;
	class PhysicEngine;
	class XmlEngine;
	class ThreadManager;
	class TaskManager;

	class FileOutput;
	class Game;
	class ResourceManager;
	class AlphaChannelManager;
	class DecoderManager;
	class EncoderManager;
	class TextManager;
	class SceneManager;

	class MENGE_API Application 
		: public MengeInterface
		, public Holder<Application>
	{
	public:
		Application( ApplicationInterface* _interface
			, const String& _userPath
			, const String& _scriptInitParams			
			, OutputStreamInterface* _platformLogger );
		~Application();

	public:
		//
		void exec( const Menge::String & _text ) override;
		void registerConsole( ConsoleInterface * _console ) override;
		//

		LogSystemInterface* initializeLogSystem();

		bool initialize( const String& _applicationFile, const String& _args, bool _loadPersonality );
		void finalize();

		void setLoggingLevel( EMessageLevel _level );
		void logMessage( const String& _message, EMessageLevel _level );
	
		void setDesktopResolution( const Resolution& _resolution );

		bool initGame();
		bool loadGame( bool _loadPersonality );

		bool createRenderWindow( WindowHandle _renderWindowHandle, WindowHandle _inputWindowHandle );

		void quit();

		const String& getPathGameFile() const;
		const String& getBaseDir() const;

	public:
		void loader( XmlElement * _xml );
		void loaderApplication( XmlElement * _xml );

	public:
		bool usePhysic() const;

		const Resolution & getCurrentResolution() const;
		const Resolution & getDesktopResolution() const;

	public:
		bool onRender();
		void onFlush();
		bool onUpdate();
		void onTick( float _timing );
		void onFocus( bool _focus );
		void onClose();
		void onMouseLeave();
		void onMouseEnter();
		bool onMouseButtonEvent( int _button, bool _isDown );
		bool onMouseMove( float _x, float _y, int _whell );
		bool onKeyEvent( unsigned int _key, unsigned int _char, bool _isDown );
		void onPaint();

		void setParticlesEnabled( bool _enabled );
		bool getParticlesEnabled() const;

		void setSoundEnabled( bool _enabled );
		bool getSoundEnabled()	const;

		bool isFocus() const;

		void minimizeWindow();
		void notifyWindowModeChanged( std::size_t _width, std::size_t _height, bool _fullscreen );

		void setMouseBounded( bool _bounded );

		unsigned int getDebugMask() const;

		void showMessageBox( const String& _message, const String& _header, unsigned int _style );

		String ansiToUtf8( const String& _ansi );
		String utf8ToAnsi( const String& _utf8 );

		String getProjectTitle() const;
		
		void setFullscreenMode( bool _fullscreen );
		bool getFullscreenMode();

		bool getHasWindowPanel() const;
		const Resolution& getResolution() const;

		static const char* getVersionInfo();

		void enableDebug( bool _enable );

		void setMousePosition( int _x, int _y );
		void injectMouseMove( int _dx, int _dy, int _dz );

		void setLanguagePack( const String& _packName );
		bool getVSync() const;
		void setMaxClientAreaSize( size_t _maxWidth, size_t _maxHeight );
		const Resolution& getMaxClientAreaSize() const;
		int getAlreadyRunningPolicy() const;
		bool getAllowFullscreenSwitchShortcut() const;
		
		void updateNotification();
		void setVSync( bool _vsync );
		void setCursorMode( bool _mode );
		bool getCursorMode() const;

	private:

		void loadPlugins_( const String& _pluginsFolder );
		void loadPlugin_( const String& _pluginName );
		void unloadPlugins_();
		void initializeSceneManager_();

	private:
		ApplicationInterface * m_interface;
		OutputStreamInterface* m_platformLogger;

		ScriptEngine * m_scriptEngine;

		ConsoleInterface * m_console;

		typedef std::vector<DynamicLibraryInterface*> TPluginVec;
		TPluginVec m_plugins;

		Game * m_game;

		String m_gameInfo;

		Resolution m_currentResolution;
		Resolution m_desktopResolution;
		Resolution m_maxClientAreaSize;

		String m_scriptInitParams;

		bool m_particles;
		bool m_sound;
		bool m_usePhysic;
		bool m_mouseBounded;
		bool m_focus;
		bool m_update;

		bool m_createRenderWindow;

		unsigned int m_debugMask;

		bool m_resetTiming;
		float m_phycisTiming;
		float m_maxTiming;

		LogEngine * m_logEngine;
		FileEngine * m_fileEngine;
		InputEngine * m_inputEngine;
		RenderEngine * m_renderEngine;
		SoundEngine * m_soundEngine;
		ParticleEngine * m_particleEngine;
		PhysicEngine * m_physicEngine;
		PhysicEngine2D * m_physicEngine2D;
		XmlEngine *	m_xmlEngine;
		ThreadManager* m_threadManager;
		TaskManager* m_taskManager;
		ResourceManager* m_resourceManager;
		AlphaChannelManager* m_alphaChannelManager;
		DecoderManager* m_decoderManager;
		EncoderManager* m_encoderManager;
		TextManager* m_textManager;
		SceneManager* m_sceneManager;

		void parseArguments_( const String& _arguments );

		String m_baseDir;
		bool m_enableDebug;
		String m_userPath;

		String m_gamePackName;
		String m_gamePackPath;
		String m_languagePackOverride;
		FileOutput* m_fileLog;
		int m_alreadyRunningPolicy;
		bool m_allowFullscreenSwitchShortcut;

		bool m_vsync;
		bool m_invalidateVsync;
		bool m_cursorMode;
		bool m_invalidateCursorMode;
	};
}
