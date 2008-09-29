#	pragma once

#	include "MengeExport.h"

#	include <string>
#	include <vector>
#	include "math/vec2.h"

#	include "Resolution.h"

#	include "Interface\ApplicationInterface.h"

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
	class ProfilerSystemInterface;

	class LogEngine;
	class ProfilerEngine;
	class FileEngine;
	class InputEngine;
	class RenderEngine;
	class SoundEngine;
	class ParticleEngine;
	class PhysicEngine2D;
	class PhysicEngine;
	class XmlEngine;

	class Game;
	class InputHandler;
	class TextField;

	class MENGE_API Application
		: public ApplicationListenerInterface
	{
	public:
		Application( ApplicationInterface* _interface );
		~Application();

	public:
		bool initialize( const String& _applicationFile, const char* _args, bool _loadPersonality );
		void finalize();
	
		void setLogSystem( LogSystemInterface * _interface );
		void setProfilerSystem( ProfilerSystemInterface * _interface );
		void setFileSystem( FileSystemInterface * _interface );
		void setInputSystem( InputSystemInterface * _interface );
		void setRenderSystem( RenderSystemInterface * _interface );
		void setSoundSystem( SoundSystemInterface * _interface );
		void setParticleSystem( ParticleSystemInterface * _interface );
		void setPhysicSystem( PhysicSystemInterface * _interface );
		void setPhysicSystem2D( PhysicSystem2DInterface * _interface );

		bool loadGame( bool _loadPersonality );
		bool createRenderWindow(WindowHandle _handle);
		bool initGame();

		void run();
		void quit();

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
		void onDestroy();
		void onWindowMovedOrResized();
		void onMouseLeave();
		void onMouseEnter();
		bool onMouseButtonEvent( int _button, bool _isDown );
		bool onMouseMove( float _x, float _y, int _whell );
		bool onKeyEvent( unsigned int _key, unsigned int _char, bool _isDown );
		void onPaint() override;

		void setParticlesEnabled( bool _enabled );
		bool getParticlesEnabled() const;

		void setSoundEnabled( bool _enabled );
		bool getSoundEnabled()	const;

		void minimizeWindow();
		void notifyWindowModeChanged( std::size_t _width, std::size_t _height, bool _fullscreen );

		void setMouseBounded( bool _bounded );

		void setFullscreenMode( bool _fullscreen );
		unsigned int getDebugMask() const;

		void showMessageBox( const String& _message, const String& _header, unsigned int _style );

	private:
		Game * m_game;
		ApplicationInterface* m_interface;

		String m_gameInfo;
		InputHandler * m_handler;

		Resolution m_currentResolution;
		Resolution m_desktopResolution;

		StringA m_commandLine;

		typedef std::vector<String> TStringVector;
		TStringVector m_resourcePaths;

		bool m_particles;
		bool m_sound;
		bool m_debugInfo;
		bool m_usePhysic;
		bool m_mouseBounded;
		bool m_hasConsole;
		bool m_verbose;
		bool m_focus;
		bool m_update;

		unsigned int m_debugMask;

		bool m_resetTiming;
		float m_phycisTiming;
		float m_maxTiming;

		LogEngine * m_logEngine;
		ProfilerEngine * m_profilerEngine;
		FileEngine * m_fileEngine;
		InputEngine * m_inputEngine;
		RenderEngine * m_renderEngine;
		SoundEngine * m_soundEngine;
		ParticleEngine * m_particleEngine;
		PhysicEngine * m_physicEngine;
		PhysicEngine2D * m_physicEngine2D;
		XmlEngine *	m_xmlEngine;

		LogSystemInterface * m_logSystem;
		ProfilerSystemInterface * m_profilerSystem;
		FileSystemInterface * m_fileSystem;
		InputSystemInterface * m_inputSystem;
		ParticleSystemInterface * m_particleSystem;
		PhysicSystem2DInterface * m_physicSystem2D;
		RenderSystemInterface * m_renderSystem;
		SoundSystemInterface * m_soundSystem;

		void parseArguments(const StringA& _arguments);
		void initPredefinedResources();
	};
}
