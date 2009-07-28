#	include "Allocator2D.h"

#	include "XmlEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Allocator2D::Allocator2D()
		: m_invalidateWorldMatrix( true )
		, m_fixedRotation( false )
		, m_origin( 0.0f, 0.0f )
		, m_position( 0.0f, 0.0f )
		, m_scale( 1.0f, 1.0f )
		, m_direction( 1.0f, 0.0f )
		, m_angle( 0.0f )
	{
		mt::ident_m3( m_localMatrix );
		mt::ident_m3( m_worldMatrix );
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator2D::invalidateWorldMatrix()
	{
		_invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator2D::_invalidateWorldMatrix()
	{
		m_invalidateWorldMatrix = true;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Allocator2D::getWorldPosition()
	{
		const mt::mat3f &wm = getWorldMatrix();

		return wm.v2.to_vec2f();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Allocator2D::getWorldDirection()
	{
		const mt::mat3f &wm = getWorldMatrix();

		return wm.v0.to_vec2f();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat3f & Allocator2D::getWorldMatrix()
	{
		return m_worldMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat3f & Allocator2D::getLocalMatrix()
	{
		if( m_invalidateWorldMatrix )
		{
			updateLocalMatrix_();
		}

		return m_localMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator2D::setLocalMatrix( const mt::mat3f & _matrix )
	{
		//assert( 0 );
		//m_position = _matrix.v2.v2;
		m_position.x = _matrix.v2.x;
		m_position.y = _matrix.v2.y;
		

		invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator2D::setLocalPosition( const mt::vec2f & _position )
	{
		m_position = _position;
		invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator2D::setLocalDirection( const mt::vec2f & _direction )
	{
		m_direction = mt::norm_safe_v2( _direction );

		m_angle = ::acosf( m_direction.x );
		if( m_direction.y < 0.0f ) m_angle = -m_angle;

		invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator2D::setRotate( float _alpha )
	{
		m_angle = _alpha;

		float cos_alpha = cosf(m_angle);
		float sin_alpha = sinf(m_angle);

		m_direction.x = cos_alpha;
		m_direction.y = sin_alpha;

		invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator2D::translate( const mt::vec2f & _delta )
	{
		m_position += _delta;

		invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat3f & Allocator2D::updateWorldMatrix( const mt::mat3f & _parentMatrix )
	{

		const mt::mat3f& localMatrix = getLocalMatrix();
		if( m_fixedRotation )
		{
			m_worldMatrix = localMatrix;
			mt::mul_v2_m3( m_worldMatrix.v2.to_vec2f(), localMatrix.v2.to_vec2f(), _parentMatrix );
		}
		else
		{
			mt::mul_m3_m3( m_worldMatrix, localMatrix, _parentMatrix );
		}

		m_invalidateWorldMatrix = false;

		_updateMatrix( _parentMatrix );

		return m_worldMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator2D::_updateMatrix( const mt::mat3f & _parentMatrix )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator2D::loader( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Transformation" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE_MEMBER( "Value", &Allocator2D::setLocalMatrix );
					XML_CASE_ATTRIBUTE_MEMBER( "Position", &Allocator2D::setLocalPosition );
					XML_CASE_ATTRIBUTE_MEMBER( "Direction", &Allocator2D::setLocalDirection );
					XML_CASE_ATTRIBUTE_MEMBER( "Rotate", &Allocator2D::setRotate );
					XML_CASE_ATTRIBUTE_MEMBER( "Origin", &Allocator2D::setOrigin );
					XML_CASE_ATTRIBUTE_MEMBER( "Scale", &Allocator2D::setScale );
				}
			}
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Scale", "Value", &Allocator2D::setScale ); // for backward compability

			XML_CASE_ATTRIBUTE_NODE( "FixedRotation", "Value", m_fixedRotation );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator2D::updateLocalMatrix_()
	{
		mt::mat3f mat_scale;
		mt::ident_m3( mat_scale );
		//mat_scale.v2.v2 = -m_origin;
		mat_scale.v2.x = -m_origin.x * m_scale.x;
		mat_scale.v2.y = -m_origin.y * m_scale.y;
		mat_scale.v0.x = m_scale.x;
		mat_scale.v1.y = m_scale.y;

		mt::mat3f mat_rot;
		mt::ident_m3( mat_rot );
		mat_rot.v0.x = m_direction.x;
		mat_rot.v0.y = m_direction.y;
		mat_rot.v1.x = -m_direction.y;
		mat_rot.v1.y = m_direction.x;

		mt::mul_m3_m3( m_localMatrix, mat_scale, mat_rot );
		//m_localMatrix.v2.v2 += m_position;
		m_localMatrix.v2.x += m_position.x;
		m_localMatrix.v2.y += m_position.y;
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator2D::setOrigin( const mt::vec2f& _origin )
	{
		m_origin = _origin;
		invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator2D::setScale( const mt::vec2f& _scale )
	{
		m_scale = _scale;
		invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator2D::setLocalPositionInt( const mt::vec2f& _position )
	{
		mt::vec2f pos( _position );
		pos.x = ::floorf( pos.x + 0.5f );
		pos.y = ::floorf( pos.y + 0.5f );
		setLocalPosition( pos );
	}
	//////////////////////////////////////////////////////////////////////////
}
