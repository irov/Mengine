#	pragma once

#	include "NodeCore.h"
#	include "NodeChildren.h"

#	include "Allocator3D-649.h"
#	include "Renderable3D.h"

namespace Menge
{
	class SceneNode3D_
		: public NodeCore
		, public NodeChildren<SceneNode3D_>
		, public Allocator3D_
		, public Renderable3D
	{
	public:
		virtual const mt::mat4f & getWorldMatrix() override;
		void changePivot() override;

	public:
		virtual void loader( XmlElement * _xml ) override;
	};
}