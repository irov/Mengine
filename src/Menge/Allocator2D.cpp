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
	bool Allocator2D::isInvalidateWorldMatrix()const
	{
		return m_invalidateWorldMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	/*const mt::vec2f & Allocator2D::getWorldPosition()
	{
		const mt::mat3f &wm = getWorldMatrix();

		return wm.v2.v2;
	}*/
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Allocator2D::getWorldDirection()
	{
		const mt::mat3f &wm = getWorldMatrix();

		return wm.v0.v2;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat3f & Allocator2D::getWorldMatrix()
	{
		return m_worldMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Allocator2D::getLocalPosition()const
	{
		return m_position;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Allocator2D::getLocalDirection()const
	{
		return m_direction;
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
		m_position = _matrix.v2.v2;

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

		invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator2D::setRotate( float _alpha )
	{
		float cos_alpha = cosf(_alpha);
		float sin_alpha = sinf(_alpha);

		m_direction[0] = cos_alpha;
		m_direction[1] = sin_alpha;

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
			mt::mul_v2_m3( m_worldMatrix.v2.v2, localMatrix.v2.v2, _parentMatrix );
		}
		else
		{
			mt::mul_m3_m3( m_worldMatrix, localMatrix, _parentMatrix );
		}

		_updateMatrix( _parentMatrix );

		m_invalidateWorldMatrix = false;

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
			XML_CASE_NODE( MENGE_TEXT("Transformation") )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE_MEMBER( MENGE_TEXT("Value"), &Allocator2D::setLocalMatrix );
					XML_CASE_ATTRIBUTE_MEMBER( MENGE_TEXT("Position"), &Allocator2D::setLocalPosition );
					XML_CASE_ATTRIBUTE_MEMBER( MENGE_TEXT("Direction"), &Allocator2D::setLocalDirection );
					XML_CASE_ATTRIBUTE_MEMBER( MENGE_TEXT("Rotate"), &Allocator2D::setRotate );
					XML_CASE_ATTRIBUTE_MEMBER( MENGE_TEXT("Origin"), &Allocator2D::setOrigin );
					XML_CASE_ATTRIBUTE_MEMBER( MENGE_TEXT("Scale"), &Allocator2D::setScale );
				}
			}
			XML_CASE_ATTRIBUTE_NODE_METHOD( MENGE_TEXT("Scale"), MENGE_TEXT("Value"), &Allocator2D::setScale ); // for backward compability

			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("FixedRotation"), MENGE_TEXT("Value"), m_fixedRotation );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator2D::updateLocalMatrix_()
	{
		mt::mat3f mat_scale;
		mt::ident_m3( mat_scale );
		mat_scale.v2.v2 = -m_origin;
		mat_scale.m[0] = m_scale.x;
		mat_scale.m[4] = m_scale.y;

		mt::mat3f mat_rot;
		mt::ident_m3( mat_rot );
		mat_rot.m[0] = m_direction[0];
		mat_rot.m[1] = m_direction[1];
		mat_rot.m[3] = -m_direction[1];
		mat_rot.m[4] = m_direction[0];

		mt::mul_m3_m3( m_localMatrix, mat_scale, mat_rot );
		m_localMatrix.v2.v2 += m_position;
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
	const mt::vec2f& Allocator2D::getOrigin() const
	{
		return m_origin;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& Allocator2D::getScale() const
	{
		return m_scale;
	}
	//////////////////////////////////////////////////////////////////////////
	float Allocator2D::getAngle() const
	{
		float angle = ::acosf( m_direction.x );
		if( m_direction.y < 0.0f ) angle = -angle;
		return angle;
	}
	//////////////////////////////////////////////////////////////////////////
}