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
		void setScene(const std::string &_name);

	public:
		bool addChildren(Node *_node) override;
		void loader(TiXmlElement *_xml) override;

	protected:		
		bool _activate()override;

	private:
		std::string m_startScene;
		Scene *m_currentScene;
	};
}