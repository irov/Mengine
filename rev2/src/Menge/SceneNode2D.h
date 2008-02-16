#	pragma once

#	include "NodeCore.h"

#	include "Renderable2D.h"

#	include "math/mat3.h"

namespace Menge
{
	class Layer2D;
	class Camera2D;

	class SceneNode2D
		: public NodeCore
		, public virtual Renderable2D
	{
	public:
		SceneNode2D();

	public:
		void setLayer( Layer2D* _layer );

		virtual mt::vec2f getScreenPosition();

		const mt::mat3f & getWorldMatrix();
		void updateMatrix( SceneNode2D * _parent );

		const mt::vec2f & getWorldPosition();
		const mt::vec2f & getWorldDirection();

		const mt::vec2f & getLocalPosition() const;
		const mt::vec2f & getLocalDirection() const;
		const mt::mat3f & getLocalMatrix() const;

		mt::vec2f & getLocalPositionModify();
		mt::vec2f & getLocalDirectionModify();
		mt::mat3f & getLocalMatrixModify();

		void setLocalMatrix( const mt::mat3f & _matrix );
		void setLocalPosition( const mt::vec2f & _position );
		void setLocalDirection( const mt::vec2f & _direction );

		void setRotate( float _alpha );
		
		void translate( const mt::vec2f & _delta );

	public:
		virtual void flip( bool _x ) {}
		virtual void setScale( const mt::vec2f& _scale ) {}
		virtual const mt::vec2f& getScale() const { static mt::vec2f s(1.0f, 1.0f); return s; }

	public:

		void update( float _timing ) override;

		void _render() override;
	
	public:
		void loader( XmlElement * _xml ) override;
		void debugRender() override;

	public:
		virtual void changePivot();
		bool isChangePivot()const;

	protected:
		virtual void _changePivot();
		virtual void _updateMatrix( SceneNode2D * _parent );

		void _addChildren( Node * _node ) override;

		Layer2D * m_layer;

	private:
		mt::mat3f m_localMatrix;
		mt::mat3f m_worldMatrix;

		bool m_changePivot;
	};
}