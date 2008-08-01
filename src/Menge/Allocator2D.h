#	pragma once

#	include "math/mat3.h"

class XmlElement;

namespace Menge
{
	class Allocator2D
	{
	public:
		Allocator2D();

	public:
		virtual const mt::mat3f & getWorldMatrix();
		const mt::mat3f & updateWorldMatrix( const mt::mat3f & _parentMatrix );

		//virtual const mt::vec2f & getWorldPosition();
		virtual const mt::vec2f & getWorldDirection();

		const mt::vec2f & getLocalPosition() const;
		const mt::vec2f & getLocalDirection() const;
		const mt::mat3f & getLocalMatrix();
		const mt::vec2f& getOrigin() const;
		const mt::vec2f& getScale() const;

		void setLocalMatrix( const mt::mat3f & _matrix );
		void setLocalPosition( const mt::vec2f & _position );
		void setLocalDirection( const mt::vec2f & _direction );
		void setOrigin( const mt::vec2f& _origin );
		void setScale( const mt::vec2f& _scale );

		void setRotate( float _alpha );
		
		void translate( const mt::vec2f & _delta );

		float getAngle() const;

	public:
		void loader( XmlElement * _xml );

	public:
		virtual void changePivot();
		bool isChangePivot()const;

	protected:
		virtual void _changePivot();
		virtual void _updateMatrix( const mt::mat3f & _parentMatrix );

	protected:
		mt::mat3f m_localMatrix;
		mt::mat3f m_worldMatrix;

		mt::vec2f m_origin;
		mt::vec2f m_position;
		mt::vec2f m_scale;
		mt::vec2f m_direction;
		void updateLocalMatrix_();

		bool m_fixedRotation;
		bool m_changePivot;
	};
}