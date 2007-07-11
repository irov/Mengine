#	pragma once

#	include "Layer.h"
#	include "SceneNode3D.h"

#	include "Viewport.h"

#	include <list>

namespace Menge
{
	class Camera3D;

	class Layer3D
		: public Layer
		, public SceneNode3D
	{
		OBJECT_DECLARE(Layer3D);

	public:
		Layer3D();

	public:
		void renderLayer() override;
	};
}