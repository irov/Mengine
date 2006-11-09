#	pragma once

#	include "MengeExport.h"

#	include <string>

namespace Menge
{
	class Game;
	class Player;
	
	class MENGE_API Application
	{
	public:
		Application();

	public:
		virtual bool init(const std::string &_xmlFile);

		virtual void run();
	
	protected:
		bool createDisplay(
			unsigned int _width, 
			unsigned int _height, 
			unsigned int _bits, 
			bool _fullScreen);

		void update();
		void render();

	private:
		unsigned int m_width;
		unsigned int m_height;
		unsigned int m_bits;
		bool m_fullScreen;

		Game * m_game;
	};
}