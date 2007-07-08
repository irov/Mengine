#	pragma once

#	include "MengeExport.h"

#	include <map>
#	include <list>

namespace Menge
{
	class Game;

	typedef std::map<int, std::list<std::string>> TPackHierarchical;

	class MENGE_API Application
	{
	public:
		Application();
		~Application();

	public:
		virtual bool init(const std::string &_xmlFile);
		virtual void run();

	private:
		bool createDisplay(int _width, int _height, int _bits, bool _fullScreen);
		void update();
		void render();
		//void	loadPlugin(const std::string& _name);

	private:
		int m_width;
		int m_height;
		int m_bits;

		bool m_fullScreen;
		bool m_debugRender;

		Game * m_game;
		
		TPackHierarchical m_packHierarchical;
	};
}