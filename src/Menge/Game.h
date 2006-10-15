#	 pragma once

#	include "Node.h"

namespace Menge
{
	class Chapter;
	class ScriptFunction;

	class Game
		: public Node
	{
		OBJECT_DECLARE(Game)
	public:
		Game();

	public:
		void setChapter(const std::string &_chapter);

	public:
		bool addChildren(Node *_node) override;
		void loader(TiXmlElement *_xml) override;		

	public:
		bool _activate()override;

	private:
		std::string m_startChapter;
		Chapter * m_currentChapter;

		ScriptFunction * m_fnInit;
		ScriptFunction * m_fnUpdate;
		ScriptFunction * m_fnRender;
	};	
}