#	pragma once

#	include "MengeExport.h"

#	include <string>

class LogSystemInterface;
class FileSystemInterface;
class InputSystemInterface;
class RenderSystemInterface;
class SoundSystemInterface;
class ParticleSystemInterface;

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
		Application();
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

		virtual void loadPak( const std::string & _pak );

		virtual bool createGame( const std::string & _game );

		virtual void quit();

	public:
		virtual int getWidth() const;
		virtual int getHeight() const;
		virtual int getBits() const;
		virtual bool isFullscreen() const;

	public:
		bool handleKeyEvent( size_t _key, bool _isDown );
		bool handleMouseButtonEvent( size_t _button, bool _isDown );
		bool handleMouseMove( int _x, int _y, int _whell );

		void frameEnded();

	private:
		bool m_quit;
		std::string m_gameInfo;
		InputHandler * m_handler;

		int m_width;
		int m_height;
		int m_bits;
		bool m_fullScreen;
	};
}