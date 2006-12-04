#	 pragma once

#	include "NodeImpl.h"

#	include "Keeper.h"

#	include <map>

namespace Menge
{
	class Player;
	class ScriptFunction;

	class Arrow;

	class Game
		: public NodeImpl
	{
		OBJECT_DECLARE(Game)
	public:
		Game();
		~Game();

	public:
		void update( float _timing );
		void render();
		
	public:
		bool addChildren(Node *_node) override;
		void loader(TiXmlElement *_xml) override;		


	public:
		void addArrow(Arrow *_arrow);
		void removeArrow(const std::string &_name);
		Arrow * getArrow(const std::string &_name);

	protected:
		bool _compile();
		void _release();
		void _debugRender();

	protected:
		std::string m_logoChapterName;
		std::string m_logoSceneName;

		std::string m_defaultArrowName;

		Player * m_player;

		ScriptFunction * m_fnInit;
		ScriptFunction * m_fnUpdate;
		ScriptFunction * m_fnRender;

		typedef std::map<std::string, Arrow *> TMapArrow;

		TMapArrow m_mapArrow;
	};	
}