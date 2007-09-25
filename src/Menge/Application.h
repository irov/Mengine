#	pragma once

#	include "MengeExport.h"

#	include <string>

class FileSystemInterface;
class InputSystemInterface;
class RenderSystemInterface;
class SoundSystemInterface;

namespace Menge
{
	class Game;
	class InputHandler;

	class MENGE_API Application
	{
	public:
		Application();
		~Application();

	public:
		virtual bool init();
		virtual void fini();
		virtual void loop();

		virtual void setFileSystem( FileSystemInterface * _interface );
		virtual void setInputSystem( InputSystemInterface * _interface );
		virtual void setRenderSystem( RenderSystemInterface * _interface );
		virtual void setSoundSystem( SoundSystemInterface * _interface );

		virtual void loadPak( const std::string & _pak, int _priority );

		virtual bool createGame( const std::string & _game );

	public:
		bool handleKeyEvent( size_t _key, bool _isDown );
		bool handleMouseButtonEvent( size_t _button, bool _isDown );
		bool handleMouseMove( int _x, int _y, int _whell );

	protected:
		void update();
		void render();
		
	private:
		int m_width;
		int m_height;
		int m_bits;
		bool m_fullScreen;

		InputHandler * m_handler;
	};
}