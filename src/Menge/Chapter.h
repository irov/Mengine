#	pragma once

#	include "NodeImpl.h"

namespace Menge
{
	class Scene;

	class Chapter
		: public NodeImpl
	{
		OBJECT_DECLARE(Chapter)

	public:
		Chapter();

	public:
		Scene * getScene( const std::string & _name );

	public:
		bool addChildren(Node *_node) override;
	};
}