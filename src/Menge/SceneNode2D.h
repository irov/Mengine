#	pragma once

#	include "NodeCore.h"
#	include "NodeChildren.h"

#	include "Allocator2D.h"
#	include "Renderable2D.h"

#	include <list>

namespace Menge
{
	class Layer2D;
	class Camera2D;

	class SceneNode2D
		: public NodeCore
		, public NodeChildren<SceneNode2D>
		, public Allocator2D
		, public Renderable2D
	{
	public:
		SceneNode2D();

	public:
		const mt::mat3f & getWorldMatrix() override;
		void changePivot() override;

		bool isVisible( const Viewport & _viewport ) override;

	public:
		virtual void render( const Viewport & _viewport );

	public:
		void loader( TiXmlElement * _xml ) override;
		void debugRender() override;
	};
}