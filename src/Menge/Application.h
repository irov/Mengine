#	pragma once

#	include "MengeExport.h"

#	include <list>

namespace Menge
{
	typedef std::list< std::pair<std::string,int> > TListLoadPaks;

	class Game;
	class ScriptEngine;
	class RenderEngine;
	class FileEngine;
	class InputEngine;
	class SoundEngine;
	class Player;

	class MENGE_API Application
	{
	public:
		Application();
		~Application();

	public:
		virtual bool init(const std::string &_xmlFile);
		virtual void run();
	private:
		int m_width;
		int m_height;
		int m_bits;
		bool m_fullScreen;

		Game * m_game;

		ScriptEngine * m_scriptEngine;
		RenderEngine * m_renderEngine;
		FileEngine * m_fileEngine;
		InputEngine * m_inputEngine;
		SoundEngine * m_soundEngine;

		bool createDisplay(int _width, int _height, int _bits, bool _fullScreen);
		void update();
		void render();
		//void	loadPlugin(const std::string& _name);
	};
}