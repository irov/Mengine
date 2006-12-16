#	pragma once

#	include "NodeImpl.h"

namespace Menge
{
	class Layer;

	class Scene
		: public NodeImpl
	{
		OBJECT_DECLARE(Scene);

	public:
		Scene();

	public:
		void loader(TiXmlElement *_xml) override;

	protected:
		bool addChildren(Node *_node) override;

	protected:
		bool _compile() override;

	private:
		std::string m_scriptFile;
		std::string	m_playListName;
	};
}