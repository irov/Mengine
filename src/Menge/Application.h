#	pragma once

#	include "MengeExport.h"

#	include <list>

namespace Menge
{
	class Game;

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

		bool createDisplay(int _width, int _height, int _bits, bool _fullScreen);
		void update();
		void render();
		//void	loadPlugin(const std::string& _name);
	};
}