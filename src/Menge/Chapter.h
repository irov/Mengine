#	pragma once

#	include "Node.h"



namespace Menge
{
	class Scene;

	class Chapter
		: public Node
	{
		OBJECT_DECLARE(Chapter)

	public:
		Chapter();

	public:
		bool addChildren(Node *_node) override;

	private:
		std::string m_startScene;
		Scene *m_currentScene;
	};
}