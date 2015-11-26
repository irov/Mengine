#	pragma once

#   include "stdex/intrusive_slug_list_size.h"
#   include "stdex/intrusive_slug.h"

#	include "math/mat4.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class Transformation3D
        : public stdex::intrusive_slug_linked<Transformation3D>
	{
	public:
		Transformation3D();

	public:
		void setRelationTransformation( Transformation3D * _relation );
		inline Transformation3D * getRelationTransformation() const;

    protected:
        void addRelationChildren_( Transformation3D * _child );
        void removeRelationChildren_( Transformation3D * _child );

	public:
		inline const mt::mat4f & getWorldMatrix() const;
		inline const mt::vec3f & getWorldPosition() const;

	public:
		inline bool isIdentityLocalMatrix() const;
		inline bool isIdentityWorldMatrix() const;

	public:
		void setWorldPosition( const mt::vec3f & _pos );

	public:
		inline const mt::mat4f & getLocalMatrix() const;

	public:
		void setLocalPosition( const mt::vec3f & _position );
		inline const mt::vec3f & getLocalPosition() const;

	public:
		void setLocalPositionX( float _x );
		float getLocalPositionX() const;
		void setLocalPositionY( float _y );
		float getLocalPositionY() const;
		void setLocalPositionZ( float _z );
		float getLocalPositionZ() const;

	public:
		void setOrigin( const mt::vec3f& _origin );
		inline const mt::vec3f& getOrigin() const;

		void setScale( const mt::vec3f& _scale );
		inline const mt::vec3f& getScale() const;

		void setOrientationX( float _angle );
		inline float getOrientationX() const;

		void setOrientationY( float _angle );
		inline float getOrientationY() const;

		void setOrientationZ( float _angle );
		inline float getOrientationZ() const;

		void setOrientation( const mt::vec3f & _orientation );
		inline const mt::vec3f & getOrientation() const;

	public:
		void setCoordinate( const mt::vec3f& _coordinate );
		inline const mt::vec3f& getCoordinate() const;

	public:
		void setTransformation( const mt::vec3f & _position, const mt::vec3f& _origin, const mt::vec3f & _coordinate, const mt::vec3f& _scale, const mt::vec3f& _orientation );
        void getTransformation( mt::vec3f & _position, mt::vec3f& _origin, mt::vec3f & _coordinate, mt::vec3f& _scale, mt::vec3f& _orientation ) const;
		void calcWorldMatrix( mt::mat4f & _wm, const mt::vec3f & _position, const mt::vec3f& _origin, const mt::vec3f & _coordinate, const mt::vec3f& _scale, const mt::vec3f& _orientation ) const;

        void resetTransformation();
		
	public:
		void translate( const mt::vec3f & _delta );
		void lookAt( const mt::vec3f & _position, const mt::vec3f & _at, const mt::vec3f & _up );

	public:
		void invalidateWorldMatrix();
		inline bool isInvalidateWorldMatrix() const;

	protected:
		virtual void _invalidateWorldMatrix();

    protected:
        void invalidateLocalMatrix();

	public:
		virtual void updateLocalMatrix() const;
		virtual void updateWorldMatrix() const;

	protected:
		static bool makeLocalMatrix_( mt::mat4f & _lm, const mt::vec3f & _position, const mt::vec3f& _origin, const mt::vec3f & _coordinate, const mt::vec3f& _scale, const mt::vec3f& _orientation );

	protected:
		Transformation3D * m_relationTransformation;

		typedef stdex::intrusive_slug_list_size<Transformation3D> TListTransformation3D;
		typedef stdex::intrusive_slug<TListTransformation3D> TSlugTransformation3D;
		TListTransformation3D m_relationChild;

		mt::vec3f m_position;
		mt::vec3f m_origin;
		mt::vec3f m_coordinate;

		mt::vec3f m_scale;
		mt::vec3f m_orientation;
				
		mutable mt::mat4f m_localMatrix;
		mutable mt::mat4f m_worldMatrix;
		mutable bool m_identityLocalMatrix;
		mutable bool m_identityWorldMatrix;
		mutable bool m_invalidateLocalMatrix;
		mutable bool m_invalidateWorldMatrix;		
	};
    //////////////////////////////////////////////////////////////////////////
    inline Transformation3D * Transformation3D::getRelationTransformation() const
    {
        return m_relationTransformation;
    }
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
	inline float Transformation3D::getOrientationX() const
	{
		return m_orientation.x;
	}
	//////////////////////////////////////////////////////////////////////////
	inline float Transformation3D::getOrientationY() const
	{
		return m_orientation.y;
	}
	//////////////////////////////////////////////////////////////////////////
	inline float Transformation3D::getOrientationZ() const
	{
		return m_orientation.z;
	}
    //////////////////////////////////////////////////////////////////////////
    inline const mt::vec3f & Transformation3D::getOrientation() const
    {
        return m_orientation;
    }
	//////////////////////////////////////////////////////////////////////////
	inline bool Transformation3D::isInvalidateWorldMatrix() const
	{
		return m_invalidateWorldMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Transformation3D::isIdentityLocalMatrix() const
	{
		if( m_invalidateLocalMatrix == true )
		{
			this->updateLocalMatrix();
		}

		return m_identityLocalMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Transformation3D::isIdentityWorldMatrix() const
	{
		if( m_invalidateWorldMatrix == true )
		{
			this->updateWorldMatrix();
		}

		return m_identityWorldMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & Transformation3D::getLocalMatrix() const
	{
		if( m_invalidateLocalMatrix == true )
		{
			this->updateLocalMatrix();
		}		

		return m_localMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & Transformation3D::getWorldMatrix() const
	{
		if( m_invalidateWorldMatrix == true )
		{
			this->updateWorldMatrix();
		}

		return m_worldMatrix;
	}
    //////////////////////////////////////////////////////////////////////////
    inline const mt::vec3f & Transformation3D::getWorldPosition() const
    {
        const mt::mat4f & wm = this->getWorldMatrix();

        const mt::vec3f & v3 = wm.v3.to_vec3f();

        return v3;
    }
}
