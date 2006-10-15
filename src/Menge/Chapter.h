#	pragma once

#	include "Node.h"



namespace Menge
{
	class Scene;

	class Chapter
		: public Node
	{
	public:
		Chapter(const std::string &_name);

	public:
		void setScene(const std::string &_name);

	public:
		bool addChildren(Node *_node) override;
		void loader(TiXmlElement *_xml) override;

	protected:		
		bool _activate()override;

	private:
		std::string m_name;

		std::string m_startScene;
		Scene *m_currentScene;
	};
}