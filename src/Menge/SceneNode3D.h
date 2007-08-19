#	pragma once

#	include "NodeCore.h"
#	include "NodeChildren.h"

#	include "Allocator3D.h"
#	include "Renderable3D.h"
#	include "Collideable3D.h"

namespace Menge
{
	class SceneNode3D
		: public NodeCore
		, public NodeChildren<SceneNode3D>
		, public Allocator3D
		, public Renderable3D
		, public Collideable3D
	{
	public:
		const mt::mat4f & getWorldMatrix() override;
		void changePivot() override;

	public:
		void loader( TiXmlElement * _xml ) override;
	};
}