#	pragma once

#	include "MengeExport.h"

#	include <string>
#	include <vector>

class LogSystemInterface;
class FileSystemInterface;
class InputSystemInterface;
class RenderSystemInterface;
class SoundSystemInterface;
class ParticleSystemInterface;
class PhysicSystemInterface;
class PhysicSystem2DInterface;


class XmlElement;

#include "Interface\ApplicationInterface.h"

namespace Menge
{
	class Game;
	class InputHandler;
	class PhysicEngine2D;

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
		void loaderConfig( XmlElement * _xml );

	public:
		virtual int getScreenWidth() const;
		virtual int getScreenHeight() const;
		virtual int getScreenBits() const;
		virtual bool isFullScreen() const;
		virtual const std::string& getRenderDriver() const;
		const std::string& getTitle() const;
		const std::string& getResourcePath() const;
		bool getVSync() const;
		bool usePhysic() const;

	public:
		void onUpdate( float _timing ) override;
		void onFocus( bool _focus ) override;
		void onClose() override;
		void onDestroy() override;
		void onWindowMovedOrResized() override;
		void onMouseLeave() override;
		void onMouseEnter() override;
		bool onMouseButtonEvent( int _button, bool _isDown ) override;
		bool onMouseMove( int _x, int _y, int _whell ) override;
		bool handleKeyEvent( size_t _key, size_t _char, bool _isDown );

		void frameStarted();
		void frameEnded();

		void setParticlesEnabled( bool _enabled );
		bool getParticlesEnabled() const;

		void setSoundEnabled( bool _enabled );
		bool getSoundEnabled()	const;

		void minimizeWindow();

	private:
		ApplicationInterface* m_interface;

		bool m_quit;
		std::string m_gameInfo;
		InputHandler * m_handler;

		int m_width;
		int m_height;
		int m_bits;
		bool m_fullScreen;
		bool m_vsync;
		std::string m_renderDriver;
		std::string m_commandLine;
		std::string m_title;
		std::string m_resourcePath;

		bool m_particles;
		bool m_sound;
		bool m_usePhysic;

		std::string m_logSystemName;
		std::string m_renderSystemName;
		std::string m_inputSystemName;
		std::string m_soundSystemName;
		std::string m_particleSystemName;
		std::string m_physicSystemName;
		std::string m_physicSystem2DName;

		PhysicEngine2D* m_physicEngine2D;

		typedef std::vector<SystemDLLInterface*> TSystemDLLVector;
		TSystemDLLVector m_systemDLLs;

		bool _initSystems();
	};
}