#	include "Transformation3D.h"

#	include "Math/angle.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Transformation3D::Transformation3D()
		: m_invalidateWorldMatrix(true)
		, m_invalidateLocalMatrix(true)
		, m_origin(0.f, 0.f)
		, m_coordinate(0.f, 0.f)
		, m_position(0.f, 0.f)
		, m_scale(1.f, 1.f)
		, m_direction(1.f, 0.f)
		, m_angle(0.f)
		, m_scaled(true)
	{
		mt::ident_m3( m_localMatrix );
		mt::ident_m3( m_worldMatrix );
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::invalidateWorldMatrix()
	{
		m_invalidateLocalMatrix = true;
		m_invalidateWorldMatrix = true;

		this->_invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::_invalidateWorldMatrix()
	{
		//Empty
	}
	////////////////////////////////////////////////////////////////////////////
	//void Transformation3D::setLocalMatrix( const mt::mat3f & _matrix )
	//{
	//	m_position.x = _matrix.v2.x;
	//	m_position.y = _matrix.v2.y;

	//	m_localMatrix = _matrix;
	//	
	//	invalidateWorldMatrix();
	//}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setLocalPosition( const mt::vec2f & _position )
	{
		if( mt::cmp_v2_v2( m_position, _position ) == true )
		{
			return;
		}

		m_position = _position;

		invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setLocalDirection( const mt::vec2f & _direction )
	{
		if( mt::cmp_v2_v2( m_direction, _direction ) == true )
		{
			return;
		}

		m_direction = _direction;
		m_angle = mt::signed_angle( _direction );

		invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setAngle( float _angle )
	{
		float norm_angle = mt::angle_norm(_angle);

		if( fabsf(m_angle - norm_angle) < 0.00001f )
		{
			return;
		}

		m_angle = norm_angle;

		mt::direction( m_direction, m_angle );

		invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Transformation3D::isScaled() const
	{
		return m_scaled;
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::resetTransformation()
	{
		m_origin = mt::vec2f(0.f, 0.f);
		m_coordinate = mt::vec2f(0.f, 0.f);
		m_position = mt::vec2f(0.f, 0.f);
		m_scale = mt::vec2f(1.f, 1.f);
		m_direction = mt::vec2f(1.f, 0.f);
		m_angle = 0.f;

		invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::translate( const mt::vec2f & _delta )
	{
		mt::vec2f new_pos = m_position + _delta;
		
		this->setLocalPosition( new_pos );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat3f & Transformation3D::updateWorldMatrix( const mt::mat3f & _parentMatrix )
	{
		m_invalidateWorldMatrix = false;

		const mt::mat3f& localMatrix = this->getLocalMatrix();

		mt::mul_m3_m3( m_worldMatrix, localMatrix, _parentMatrix );

		return m_worldMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::updateLocalMatrix_()
	{
		m_invalidateLocalMatrix = false;

		mt::mat3f mat_scale;
		mt::ident_m3( mat_scale );
		mat_scale.v2.x = -(m_origin.x + m_coordinate.x) * m_scale.x;
		mat_scale.v2.y = -(m_origin.y + m_coordinate.y) * m_scale.y;
		mat_scale.v0.x = m_scale.x;
		mat_scale.v1.y = m_scale.y;

		mt::mat3f mat_rot;
		mt::ident_m3( mat_rot );
		mat_rot.v0.x = m_direction.x;
		mat_rot.v0.y = m_direction.y;
		mat_rot.v1.x = -m_direction.y;
		mat_rot.v1.y = m_direction.x;

		mt::mul_m3_m3( m_localMatrix, mat_scale, mat_rot );
		m_localMatrix.v2.x += m_position.x + m_coordinate.x;
		m_localMatrix.v2.y += m_position.y + m_coordinate.y;
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setOrigin( const mt::vec2f& _origin )
	{
		if( mt::cmp_v2_v2( m_origin, _origin ) == true )
		{
			return;
		}
		
		m_origin = _origin;

		this->invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setCoordinate( const mt::vec2f& _coordinate )
	{
		if( mt::cmp_v2_v2( m_coordinate, _coordinate ) == true )
		{
			return;
		}

		m_coordinate = _coordinate;

		this->invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setScale( const mt::vec2f& _scale )
	{
		if( mt::cmp_v2_v2( m_scale, _scale ) == true )
		{
			return;
		}

		m_scale = _scale;
		
		this->invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
}
