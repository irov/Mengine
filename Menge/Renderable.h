#	pragma once

#	include "Allocator.h"

#	include "math/mat3.h"

namespace RvEngine
{
	class RenderEngine;

	class Renderable
		: public Allocator
	{
		OBJECT_DECLARE(Renderable);

	public:
		virtual void render();

	public:
		void setSceneManager(SceneManager *sceneManager)override;

	protected:
		RenderEngine *m_renderEngine;
	};
}