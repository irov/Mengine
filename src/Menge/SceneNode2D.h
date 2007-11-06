#	pragma once

#	include "NodeCore.h"
#	include "NodeChildren.h"

#	include "Allocator2D.h"
#	include "Renderable2D.h"
#	include "Collideable2D.h"

namespace Menge
{
	class Layer2D;
	class Camera2D;

	class SceneNode2D
		: public NodeCore
		, public NodeChildren<SceneNode2D>
		, public Allocator2D
		, public virtual Renderable2D
		, public Collideable2D
	{
	public:
		SceneNode2D();

	public:
		const mt::mat3f & getWorldMatrix() override;
		void changePivot() override;

	public:
		void _render() override;
	
	public:
		void loader( TiXmlElement * _xml ) override;
		void debugRender() override;
	};
}