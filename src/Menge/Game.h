#	 pragma once

#	include "Keeper.h"

#	include <map>

class TiXmlElement;

namespace Menge
{
	class Player;
	class ScriptFunction;
	
	class Chapter;
	class Arrow;

	class BackgroundSound;

	class Game
	{
	public:
		Game();
		~Game();

	public:
		void playList(const std::string& _name);

	public:
		void update( float _timing );
		void render();

	public:
		bool compile();
		void release();
		void debugRender();
		
	public:
		void loader(TiXmlElement *_xml);		

	public:
		void addArrow(Arrow *_arrow);
		void removeArrow(const std::string &_name);
		Arrow * getArrow(const std::string &_name);

	public:
		void addChapter(Chapter * _chapter);
		Chapter * getChapter(const std::string &_name);


	protected:
		std::string m_logoChapterName;
		std::string m_logoSceneName;

		std::string m_defaultArrowName;

		Player * m_player;

		BackgroundSound * m_backsoundManager;

		ScriptFunction * m_fnInit;
		ScriptFunction * m_fnUpdate;
		ScriptFunction * m_fnRender;

		typedef std::map<std::string, Arrow *> TMapArrow;
		TMapArrow m_mapArrow;

		typedef std::map<std::string, Chapter *> TMapChapter;
		TMapChapter m_mapChapter;
	};	
}