#	include "Transformation3D.h"

#	include "Math/angle.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Transformation3D::Transformation3D()
		: m_invalidateWorldMatrix(true)
		, m_invalidateLocalMatrix(true)
		, m_origin(0.f, 0.f, 0.f)
		, m_coordinate(0.f, 0.f, 0.f)
		, m_position(0.f, 0.f, 0.f)
		, m_scale(1.f, 1.f, 1.f)
		, m_rotation(0.f, 0.f, 0.f)
		, m_relationTransformation(0)
	{
		mt::ident_m4( m_localMatrix );
		mt::ident_m4( m_worldMatrix );
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setRelationTransformation( Transformation3D * _relationTransformation )
	{
		m_relationTransformation = _relationTransformation;
	}
	//////////////////////////////////////////////////////////////////////////
	Transformation3D * Transformation3D::getRelationTransformation() const
	{
		return m_relationTransformation;
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
	void Transformation3D::setLocalPosition( const mt::vec3f & _position )
	{
		if( mt::cmp_v3_v3( m_position, _position ) == true )
		{
			return;
		}

		m_position = _position;

		this->invalidateWorldMatrix();
	}
	////////////////////////////////////////////////////////////////////////////
	//void Transformation3D::setLocalDirection( const mt::vec3f & _direction )
	//{
	//	if( mt::cmp_v3_v3( m_direction, _direction ) == true )
	//	{
	//		return;
	//	}

	//	m_direction = _direction;
	//	m_angle = mt::signed_angle( _direction );

	//	this->invalidateWorldMatrix();
	//}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setRotateX( float _angle )
	{
		float norm_angle = mt::angle_norm(_angle);

		if( fabsf(m_rotation.x - norm_angle) < 0.00001f )
		{
			return;
		}

		m_rotation.x = norm_angle;

		this->invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setRotateY( float _angle )
	{
		float norm_angle = mt::angle_norm(_angle);

		if( fabsf(m_rotation.y - norm_angle) < 0.00001f )
		{
			return;
		}

		m_rotation.y = norm_angle;

		this->invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setRotateZ( float _angle )
	{
		float norm_angle = mt::angle_norm(_angle);

		if( fabsf(m_rotation.z - norm_angle) < 0.00001f )
		{
			return;
		}

		m_rotation.z = norm_angle;

		this->invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setRotation( const mt::vec3f & _rotate )
	{
		m_rotation = _rotate;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & Transformation3D::getRotation() const
	{
		return m_rotation;
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setTransformation( const mt::vec3f & _position, const mt::vec3f& _origin, const mt::vec3f& _scale, const mt::vec3f& _rotate )
	{
		m_position = _position;
		m_origin = _origin;
		m_scale = _scale;
		m_rotation = _rotate;

		this->invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::resetTransformation()
	{
		m_origin = mt::vec3f(0.f, 0.f, 0.f);
		m_coordinate = mt::vec3f(0.f, 0.f, 0.f);
		m_position = mt::vec3f(0.f, 0.f, 0.f);
		m_scale = mt::vec3f(1.f, 1.f, 1.f);
		m_rotation = mt::vec3f(0.f, 0.f, 0.f);

		this->invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::translate( const mt::vec3f & _delta )
	{
		mt::vec3f new_pos = m_position + _delta;
		
		this->setLocalPosition( new_pos );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f & Transformation3D::updateWorldMatrix( const mt::mat4f & _parentMatrix ) const
	{
		m_invalidateWorldMatrix = false;

		const mt::mat4f & localMatrix = this->getLocalMatrix();

		mt::mul_m4_m4( m_worldMatrix, localMatrix, _parentMatrix );

		return m_worldMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::updateLocalMatrix_() const
	{
		m_invalidateLocalMatrix = false;

		mt::mat4f mat_scale;
		mt::ident_m4( mat_scale );
		mat_scale.v3.x = -(m_origin.x + m_coordinate.x) * m_scale.x;
		mat_scale.v3.y = -(m_origin.y + m_coordinate.y) * m_scale.y;
		mat_scale.v3.z = -(m_origin.z + m_coordinate.z) * m_scale.z;
		mat_scale.v0.x = m_scale.x;
		mat_scale.v1.y = m_scale.y;
		mat_scale.v2.z = m_scale.z;
		
		mt::mat4f mat_rot;
		mt::make_rotate_m4( mat_rot, m_rotation.x, m_rotation.y, m_rotation.z );
		//mat_rot.v0.x = m_direction.x;
		//mat_rot.v0.y = m_direction.y;
		//mat_rot.v1.x = -m_direction.y;
		//mat_rot.v1.y = m_direction.x;

		mt::mul_m4_m4( m_localMatrix, mat_scale, mat_rot );

		m_localMatrix.v3.x += m_position.x + m_coordinate.x;
		m_localMatrix.v3.y += m_position.y + m_coordinate.y;
		m_localMatrix.v3.z += m_position.z + m_coordinate.z;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f & Transformation3D::getWorldMatrix() const
	{
		if( m_relationTransformation == 0 )
		{
			return this->getLocalMatrix();
		}

		if( this->isInvalidateWorldMatrix() == false )
		{
			return this->getRelationMatrix();
		}

		const mt::mat4f & wm = m_relationTransformation->getWorldMatrix();

		const mt::mat4f & update_wm = this->updateWorldMatrix( wm );

		return update_wm;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & Transformation3D::getWorldPosition() const
	{
		const mt::mat4f &wm = this->getWorldMatrix();

		return wm.v3.to_vec3f();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setWorldPosition( const mt::vec3f & _pos )
	{
		const mt::vec3f & wp = this->getWorldPosition();

		mt::vec3f wp_offset = _pos - wp;

		this->translate( wp_offset );
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setOrigin( const mt::vec3f& _origin )
	{
		if( mt::cmp_v3_v3( m_origin, _origin ) == true )
		{
			return;
		}
		
		m_origin = _origin;

		this->invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setCoordinate( const mt::vec3f& _coordinate )
	{
		if( mt::cmp_v3_v3( m_coordinate, _coordinate ) == true )
		{
			return;
		}

		m_coordinate = _coordinate;

		this->invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setScale( const mt::vec3f& _scale )
	{
		if( mt::cmp_v3_v3( m_scale, _scale ) == true )
		{
			return;
		}

		if( m_scale.z == 0.f )
		{
			return;
		}

		m_scale = _scale;
		
		this->invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
}
