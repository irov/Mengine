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
		void setLayer( Layer2D* _layer );
		virtual mt::vec2f getScreenPosition();
		const mt::mat3f & getWorldMatrix() override;
		void changePivot() override;
		
		virtual void flip( bool _x ) {}
		virtual void setScale( const mt::vec2f& _scale ) {}

	public:
		void _render() override;
	
	public:
		void loader( TiXmlElement * _xml ) override;
		void debugRender() override;

	protected:
		Layer2D* m_layer;

		void _addChildren( SceneNode2D * _node ) override;

		void	save() override;
	};
}