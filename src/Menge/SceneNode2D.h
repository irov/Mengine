#	pragma once

#	include "NodeCore.h"
#	include "NodeChildren.h"

#	include "Allocator2D.h"
#	include "Renderable2D.h"

namespace Menge
{
	class Layer2D;
	class Camera2D;

	class SceneNode2D
		: public NodeCore
		, public NodeChildren<SceneNode2D>
		, public Allocator2D
		, public virtual Renderable2D
	{
	public:
		SceneNode2D();

	public:
		void setLayer( Layer2D* _layer );
		Layer2D* getLayer() const;
		virtual mt::vec2f getScreenPosition();
		const mt::mat3f & getWorldMatrix() override;
		void changePivot() override;
		
		virtual void flip( bool _x ) {}
		virtual void setScale( const mt::vec2f& _scale ) {}
		virtual void alphaTo( float _alpha, float _time );
		virtual void setAlpha( float _alpha );
		virtual const mt::vec2f& getScale() const { static mt::vec2f s(1.0f, 1.0f); return s; }
		virtual void setListener( PyObject* _listener ) override;

	public:

		//void update( float _timing ) override;

		void _render( bool _enableDebug ) override;
		//void _update( float _timing ) override;
	
	public:
		void loader( XmlElement * _xml ) override;
		void debugRender() override;

	protected:
		Layer2D * m_layer;
		//PyObject * m_listener;

		void _addChildren( SceneNode2D * _node ) override;
		//virtual void _onSetListener();
	};
}