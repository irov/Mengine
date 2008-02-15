#	pragma once

#	include "MengeExport.h"

#	include <string>

class LogSystemInterface;
class FileSystemInterface;
class InputSystemInterface;
class RenderSystemInterface;
class SoundSystemInterface;
class ParticleSystemInterface;
class PhysicSystemInterface;

class XmlElement;

namespace Menge
{
	class Game;
	class InputHandler;

	//! Application - .
	/*! 
	* 
	*/

	class MENGE_API Application
	{
	public:
		Application( const std::string& _args );
		~Application();

	public:
		virtual bool initialize( const std::string & _applicationFile );
		virtual void finalize();
		virtual bool update( float _timing );

		virtual void setLogSystem( LogSystemInterface * _interface );
		virtual void setFileSystem( FileSystemInterface * _interface );
		virtual void setInputSystem( InputSystemInterface * _interface );
		virtual void setRenderSystem( RenderSystemInterface * _interface );
		virtual void setSoundSystem( SoundSystemInterface * _interface );
		virtual void setParticleSystem( ParticleSystemInterface * _interface );
		virtual void setPhysicSystem( PhysicSystemInterface * _interface );

		virtual void loadPak( const std::string & _pak );

		virtual bool createGame();

		virtual void quit();

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
		bool handleKeyEvent( size_t _key, size_t _char, bool _isDown );
		bool handleMouseButtonEvent( size_t _button, bool _isDown );
		bool handleMouseMove( int _x, int _y, int _whell );
		void handleMouseLeave();
		void handleMouseEnter();

		void frameStarted();
		void frameEnded();

		void setParticlesEnabled( bool _enabled );
		bool getParticlesEnabled() const;

		void setSoundEnabled( bool _enabled );
		bool getSoundEnabled()	const;

	private:
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
		bool m_fixedContentResolution;
		std::string m_title;
		std::string m_resourcePath;

		bool m_particles;
		bool m_sound;
		bool m_usePhysic;
	};
}