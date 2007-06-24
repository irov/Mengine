#	pragma once

#	include "Allocator2D.h"

namespace Menge
{
	class Scene;

	class Entity
		: public Allocator2D
	{
		OBJECT_DECLARE( Entity )

	public:
		~Entity();

	public:
		void destroy() override;

	public:
		bool addChildren( Node * _node ) override;

	protected:
		bool _activate();

	};
}