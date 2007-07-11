#	pragma once

#	include "MengeExport.h"

#	include <map>
#	include <list>

class FileSystemInterface;
class InputSystemInterface;
class RenderSystemInterface;
class SoundSystemInterface;

namespace Menge
{
	class Game;

	class MENGE_API Application
	{
	public:
		Application();
		~Application();

	public:
		virtual bool init();
		virtual void loop();

		virtual void setFileSystem( FileSystemInterface * _interface );
		virtual void setInputSystem( InputSystemInterface * _interface );
		virtual void setRenderSystem( RenderSystemInterface * _interface );
		virtual void setSoundSystem( SoundSystemInterface * _interface );

		virtual void loadPak( const std::string & _pak, int _priority );

		virtual bool createGame( const std::string & _game );

	private:
		void update();
		void render();
		//void	loadPlugin(const std::string& _name);

	private:
		int m_width;
		int m_height;
		int m_bits;

		bool m_fullScreen;

		Game * m_game;
		
		
	};
}