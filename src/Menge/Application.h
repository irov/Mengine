#	pragma once

#	include "MengeExport.h"

#	include <string>
#	include <vector>
#	include "math/vec2.h"

class XmlElement;


#include "Interface\ApplicationInterface.h"

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

	class LogEngine;
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

	//! Application - .
	/*! 
	* 
	*/

	class MENGE_API Application
		: public ApplicationListenerInterface
	{
	public:
		Application( ApplicationInterface* _interface );
		~Application();

	public:
		bool initialize( const std::string & _applicationFile, const std::string& _args );
		void finalize();
		bool update( float _timing );

		void setLogSystem( LogSystemInterface * _interface );
		void setFileSystem( FileSystemInterface * _interface );
		void setInputSystem( InputSystemInterface * _interface );
		void setRenderSystem( RenderSystemInterface * _interface );
		void setSoundSystem( SoundSystemInterface * _interface );
		void setParticleSystem( ParticleSystemInterface * _interface );
		void setPhysicSystem( PhysicSystemInterface * _interface );
		void setPhysicSystem2D( PhysicSystem2DInterface * _interface );

		void loadPak( const std::string & _pak );

		bool createGame();

		void run();
		void quit();

	public:
		void loader( XmlElement * _xml );
		void loaderApplication( XmlElement * _xml );

	public:
		bool usePhysic() const;
		const mt::vec2f& getCurrentResolution() const;

	public:
		void onUpdate( float _timing ) override;
		void onFocus( bool _focus ) override;
		void onActive( bool _active ) override;
		void onClose() override;
		void onDestroy() override;
		void onWindowMovedOrResized() override;
		void onMouseLeave() override;
		void onMouseEnter() override;
		bool onMouseButtonEvent( int _button, bool _isDown ) override;
		bool onMouseMove( float _x, float _y, int _whell ) override;
		bool onKeyEvent( unsigned int _key, unsigned int _char, bool _isDown );

		void frameStarted();
		void frameEnded();

		void setParticlesEnabled( bool _enabled );
		bool getParticlesEnabled() const;

		void setSoundEnabled( bool _enabled );
		bool getSoundEnabled()	const;

		void minimizeWindow();
		void notifyWindowModeChanged( std::size_t _width, std::size_t _height, bool _fullscreen );

		void setMouseBounded( bool _bounded );

		void setFullscreenMode( bool _fullscreen );
		const mt::vec2f& getDesktopResolution() const;

		bool isDebugRender() const;
	private:
		ApplicationInterface* m_interface;

		std::string m_gameInfo;
		InputHandler * m_handler;

		mt::vec2f m_currentResolution;
		std::string m_commandLine;

		typedef std::vector<std::string> TStringVector;
		TStringVector m_resourcePaths;

		bool m_particles;
		bool m_sound;
		bool m_debugRender;
		bool m_debugInfo;
		bool m_usePhysic;
		bool m_mouseBounded;

		float m_phycisTiming;
		bool m_resetTiming;
		float m_maxTiming;

		std::string m_debugResourcesPath;
		TextField* m_debugTextField;
		char m_debugText[128];
		float m_FPS;

		LogEngine* m_logEngine;
		FileEngine* m_fileEngine;
		InputEngine* m_inputEngine;
		RenderEngine* m_renderEngine;
		SoundEngine* m_soundEngine;
		ParticleEngine* m_particleEngine;
		PhysicEngine * m_physicEngine;
		PhysicEngine2D * m_physicEngine2D;
		XmlEngine*	m_xmlEngine;

		mt::vec2f m_desktopResolution;
	};
}
