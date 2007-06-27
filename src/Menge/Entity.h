#	pragma once

#	include "SceneNode2D.h"

namespace Menge
{
	class Scene;

	class Entity
		: public SceneNode2D
	{
		OBJECT_DECLARE( Entity )

	public:
		~Entity();

	public:
		void destroy() override;


	protected:
		bool _activate();

	};
}