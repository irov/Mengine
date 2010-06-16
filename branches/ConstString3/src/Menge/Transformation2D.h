#	pragma once

#	include "Loadable.h"

#	include "math/mat3.h"

namespace Menge
{
	class Transformation2D
		: virtual public Loadable
	{
	public:
		Transformation2D();

	public:
		inline const mt::mat3f & getLocalMatrix();

		inline const mt::vec2f & getLocalPosition() const;
		inline const mt::vec2f & getLocalDirection() const;

		inline const mt::mat3f & getRelationMatrix() const;

		inline const mt::vec2f& getOrigin() const;
		inline const mt::vec2f& getScale() const;
		inline float getAngle() const;

	public:
		void setLocalMatrix( const mt::mat3f & _matrix );
		void setLocalPosition( const mt::vec2f & _position );
		void setLocalPositionInt( const mt::vec2f& _position );
		void setLocalDirection( const mt::vec2f & _direction );
		void setOrigin( const mt::vec2f& _origin );
		void setScale( const mt::vec2f& _scale );

		void setAngle( float _alpha );
		void setFixedRotation( bool _fixed );
		
	public:
		void translate( const mt::vec2f & _delta );

	public:
		void loader( XmlElement * _xml ) override;
		void parser( BinParser * _parser ) override;

	public:
		void invalidateWorldMatrix();
		inline bool isInvalidateWorldMatrix() const;

	protected:
		const mt::mat3f & updateWorldMatrix( const mt::mat3f & _parentMatrix );

	protected:
		virtual void _invalidateWorldMatrix();

	protected:
		void updateLocalMatrix_();

	protected:
		mt::mat3f m_localMatrix;

		mt::vec2f m_origin;
		mt::vec2f m_position;
		mt::vec2f m_scale;
		mt::vec2f m_direction;
		float m_angle;

		bool m_fixedRotation;
		
		bool m_invalidateWorldMatrix;
		bool m_invalidateLocalMatrix;
		mt::mat3f m_worldMatrix;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const mt::vec2f & Transformation2D::getLocalPosition()const
	{
		return m_position;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::vec2f & Transformation2D::getLocalDirection()const
	{
		return m_direction;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::vec2f& Transformation2D::getOrigin() const
	{
		return m_origin;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::vec2f& Transformation2D::getScale() const
	{
		return m_scale;
	}
	//////////////////////////////////////////////////////////////////////////
	inline float Transformation2D::getAngle() const
	{
		return m_angle;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat3f & Transformation2D::getRelationMatrix() const
	{
		return m_worldMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Transformation2D::isInvalidateWorldMatrix()const
	{
		return m_invalidateWorldMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat3f & Transformation2D::getLocalMatrix()
	{
		if( m_invalidateLocalMatrix == true )
		{
			updateLocalMatrix_();
		}		

		return m_localMatrix;
	}
}
