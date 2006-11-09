#	 pragma once

#	include "Node.h"

#	include "Keeper.h"

namespace Menge
{
	class Player;
	class ScriptFunction;

	class Game
		: public Node
	{
		OBJECT_DECLARE(Game)
	public:
		Game();

	public:
		void update( float _timing );
		void render();
		
	public:
		bool addChildren(Node *_node) override;
		void loader(TiXmlElement *_xml) override;		

	protected:
		bool _compile();

	protected:
		std::string m_logoChapterName;
		std::string m_logoSceneName;

		Player * m_player;

		ScriptFunction * m_fnInit;
		ScriptFunction * m_fnUpdate;
		ScriptFunction * m_fnRender;
	};	
}