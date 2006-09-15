#	pragma once

#	include "Node.h"

#	include <map>

namespace RvEngine
{
	class Scene;

	class Chapter
		: public Node
	{
		OBJECT_DECLARE(Chapter);

	public:
		Scene * createScene(const std::string &_name);
		Scene * getScene(const std::string &_name);
		void removeScene(Scene *_scene);

	public:
		void loader(TiXmlElement *_xml)override;
	};
}