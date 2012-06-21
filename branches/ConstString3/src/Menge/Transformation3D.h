#	pragma once

#	include "math/mat4.h"

namespace Menge
{
	class Transformation3D
	{
	public:
		Transformation3D();

	public:
		void setRelationTransformation( Transformation3D * _relationTransformation );
		Transformation3D * getRelationTransformation() const;

	public:
		const mt::mat4f & getWorldMatrix() const;
		const mt::vec3f & getWorldPosition() const;

	public:
		void setWorldPosition( const mt::vec3f & _pos );

	public:
		inline const mt::mat4f & getRelationMatrix() const;
		
	public:
		//void setLocalMatrix( const mt::mat3f & _matrix );
		inline const mt::mat4f & getLocalMatrix() const;

	public:
		void setLocalPosition( const mt::vec3f & _position );
		inline const mt::vec3f & getLocalPosition() const;

		//void setLocalDirection( const mt::vec3f & _direction );
		//inline const mt::vec3f & getLocalDirection() const;

		void setOrigin( const mt::vec3f& _origin );
		inline const mt::vec3f& getOrigin() const;

		void setScale( const mt::vec3f& _scale );
		inline const mt::vec3f& getScale() const;

		void setRotateX( float _angle );
		inline float getRotateX() const;

		void setRotateY( float _angle );
		inline float getRotateY() const;

		void setRotateZ( float _angle );
		inline float getRotateZ() const;

		void setRotation( const mt::vec3f & _rotation );
		const mt::vec3f & getRotation() const;

	public:
		void setCoordinate( const mt::vec3f& _coordinate );
		inline const mt::vec3f& getCoordinate() const;

	public:
		void setTransformation( const mt::vec3f & _position, const mt::vec3f& _origin, const mt::vec3f& _scale, const mt::vec3f& _rotate );
		void resetTransformation();
		
	public:
		void translate( const mt::vec3f & _delta );

	public:
		void invalidateWorldMatrix();
		inline bool isInvalidateWorldMatrix() const;

	protected:
		const mt::mat4f & updateWorldMatrix( const mt::mat4f & _parentMatrix ) const;

	protected:
		virtual void _invalidateWorldMatrix();

	protected:
		void updateLocalMatrix_() const;

	protected:
		mt::vec3f m_origin;
		mt::vec3f m_coordinate;
		mt::vec3f m_position;
		mt::vec3f m_scale;

		mt::vec3f m_rotation;

		Transformation3D * m_relationTransformation;

		mutable mt::mat4f m_localMatrix;
		mutable mt::mat4f m_worldMatrix;
		mutable bool m_invalidateWorldMatrix;
		mutable bool m_invalidateLocalMatrix;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const mt::vec3f & Transformation3D::getLocalPosition()const
	{
		return m_position;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::vec3f& Transformation3D::getOrigin() const
	{
		return m_origin;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::vec3f& Transformation3D::getCoordinate() const
	{
		return m_coordinate;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::vec3f& Transformation3D::getScale() const
	{
		return m_scale;
	}
	//////////////////////////////////////////////////////////////////////////
	inline float Transformation3D::getRotateX() const
	{
		return m_rotation.x;
	}
	//////////////////////////////////////////////////////////////////////////
	inline float Transformation3D::getRotateY() const
	{
		return m_rotation.y;
	}
	//////////////////////////////////////////////////////////////////////////
	inline float Transformation3D::getRotateZ() const
	{
		return m_rotation.z;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & Transformation3D::getRelationMatrix() const
	{
		return m_worldMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Transformation3D::isInvalidateWorldMatrix()const
	{
		return m_invalidateWorldMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & Transformation3D::getLocalMatrix() const
	{
		if( m_invalidateLocalMatrix == true )
		{
			updateLocalMatrix_();
		}		

		return m_localMatrix;
	}
}
