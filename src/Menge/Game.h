#	pragma once

#	include "Holder.h"

#	include <map>

class TiXmlElement;

namespace Menge
{
	class Player;

	class Chapter;
	class Arrow;

	class WinAmp;
	class DialogManager;

	class Game
	{
	public:
		Game();
		~Game();

	public:
		void update( float _timing );
		void render();
		void loader(TiXmlElement *_xml);

	public:
		bool init();
		void release();
		void debugRender();

	public:
		void addArrow(Arrow *_arrow);
		void removeArrow(const std::string &_name);
		Arrow * getArrow(const std::string &_name);
		Arrow * getDefaultArrow();

	public:
		void addChapter(Chapter * _chapter);
		Chapter * getChapter(const std::string &_name);

	public:
		void test( const char * _text );

	protected:
		std::string m_defaultArrowName;

		std::string m_personality;

		std::string m_eventInit;
		std::string m_eventUpdate;
		std::string m_eventFini;

		Player * m_player;

		Arrow * m_defaultArrow;
		WinAmp * m_winAmp;
		DialogManager * m_dialogManager;

		typedef std::map<std::string, Arrow*> TMapArrow;
		TMapArrow m_mapArrow;

		typedef std::map<std::string, Chapter*> TMapChapter;
		TMapChapter m_mapChapter;
	};	
}
