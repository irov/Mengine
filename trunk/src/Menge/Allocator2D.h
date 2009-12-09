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

		const mt::vec2f & getWorldPosition();
		const mt::vec2f & getWorldDirection();

		inline const mt::mat3f & getLocalMatrix();

		inline const mt::vec2f & getLocalPosition() const;
		inline const mt::vec2f & getLocalDirection() const;

		inline const mt::vec2f& getOrigin() const;
		inline const mt::vec2f& getScale() const;
		inline float getAngle() const;

		void setLocalMatrix( const mt::mat3f & _matrix );
		void setLocalPosition( const mt::vec2f & _position );
		void setLocalPositionInt( const mt::vec2f& _position );
		void setLocalDirection( const mt::vec2f & _direction );
		void setOrigin( const mt::vec2f& _origin );
		void setScale( const mt::vec2f& _scale );

		void setAngle( float _alpha );
		
		void translate( const mt::vec2f & _delta );

	public:
		void loader( XmlElement * _xml );

	public:
		void invalidateWorldMatrix();
		inline bool isInvalidateWorldMatrix() const;

	protected:
		virtual void _invalidateWorldMatrix();
		//virtual void _updateWorldMatrix( const mt::mat3f & _parentMatrix );

	protected:
		void updateLocalMatrix_();

	protected:
		mt::mat3f m_localMatrix;
		mt::mat3f m_worldMatrix;

		mt::vec2f m_origin;
		mt::vec2f m_position;
		mt::vec2f m_scale;
		mt::vec2f m_direction;
		float m_angle;
		
		bool m_fixedRotation;
		bool m_invalidateWorldMatrix;
		bool m_invalidateLocalMatrix;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const mt::vec2f & Allocator2D::getLocalPosition()const
	{
		return m_position;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::vec2f & Allocator2D::getLocalDirection()const
	{
		return m_direction;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::vec2f& Allocator2D::getOrigin() const
	{
		return m_origin;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::vec2f& Allocator2D::getScale() const
	{
		return m_scale;
	}
	//////////////////////////////////////////////////////////////////////////
	inline float Allocator2D::getAngle() const
	{
		return m_angle;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Allocator2D::isInvalidateWorldMatrix()const
	{
		return m_invalidateWorldMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat3f & Allocator2D::getLocalMatrix()
	{
		if( m_invalidateLocalMatrix == true )
		{
			updateLocalMatrix_();
		}		

		return m_localMatrix;
	}
}
