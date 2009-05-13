#	pragma once

#	include "MengeExport.h"

#	include "Resolution.h"

#	include "Interface/ApplicationInterface.h"

class XmlElement;

namespace Menge
{
	class FileSystemInterface;
	class InputSystemInterface;
	class RenderSystemInterface;
	class SoundSystemInterface;
	class ParticleSystemInterface;
	class PhysicSystemInterface;
	class PhysicSystem2DInterface;
	class LogSystemInterface;
	class ThreadSystemInterface;
	
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

	class OutStreamInterface;
	class Game;

	class MENGE_API Application 
		: public MengeInterface
	{
	public:
		Application( ApplicationInterface* _interface, const String& _userPath, bool _userLocal, const String& _scriptInitParams );
		~Application();

	public:
		//
		void exec( const Menge::String & _text ) override;
		void registerConsole( ConsoleInterface * _console ) override;
		LogSystemInterface* getLogSystem() const override;
		RenderSystemInterface* getRenderSystem() const override;
		//

		LogSystemInterface* initializeLogSystem();

		bool initialize( const String& _applicationFile, const String& _args, bool _loadPersonality );
		void finalize();
	
		void setLogSystem( LogSystemInterface * _interface );
		void setFileSystem( FileSystemInterface * _interface );
		void setInputSystem( InputSystemInterface * _interface );
		void setRenderSystem( RenderSystemInterface * _interface );
		void setSoundSystem( SoundSystemInterface * _interface );
		void setParticleSystem( ParticleSystemInterface * _interface );
		void setPhysicSystem( PhysicSystemInterface * _interface );
		void setPhysicSystem2D( PhysicSystem2DInterface * _interface );

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
		void onUpdate( float _timing );
		void onFocus( bool _focus );
		void onClose();
		void onWindowMovedOrResized();
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
	private:

		void loadPlugins_( const String& _pluginsFolder );
		void loadPlugin_( const String& _pluginName );
		void unloadPlugins_();

		ScriptEngine * m_scriptEngine;

		ConsoleInterface * m_console;

		typedef std::vector<DynamicLibraryInterface*> TPluginVec;
		TPluginVec m_plugins;

		Game * m_game;
		ApplicationInterface * m_interface;

		String m_gameInfo;

		Resolution m_currentResolution;
		Resolution m_desktopResolution;

		String m_scriptInitParams;

		bool m_particles;
		bool m_sound;
		bool m_debugInfo;
		bool m_usePhysic;
		bool m_mouseBounded;
		bool m_focus;
		bool m_update;

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

		LogSystemInterface * m_logSystem;
		FileSystemInterface * m_fileSystem;
		InputSystemInterface * m_inputSystem;
		ParticleSystemInterface * m_particleSystem;
		PhysicSystem2DInterface * m_physicSystem2D;
		RenderSystemInterface * m_renderSystem;
		SoundSystemInterface * m_soundSystem;
		ThreadSystemInterface* m_threadSystem;

		void parseArguments_( const String& _arguments );

		String m_baseDir;
		bool m_enableDebug;
		bool m_userLocal;
		String m_userPath;

		int m_altDown;

		String m_gamePack;
		String m_languagePackOverride;
		OutStreamInterface* m_fileLog;
	};
}
