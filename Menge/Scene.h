#	pragma once

#	include "Node.h"

#	include <list>

namespace RvEngine
{
	class Layer;

	typedef std::list<Layer *> TListLayer;

	class Scene
		: public Node
	{
		OBJECT_DECLARE(Scene);

	public:
		Layer * createLayer(const std::string &_nameLayer);
		Layer * getLayer(const std::string &_nameLayer);

	protected:
		bool addChildren(Node *_node) override;

	public:
		void loader(TiXmlElement *_xml);
	};
}