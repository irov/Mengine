#	include "Transformation2D.h"

#	include "XmlEngine.h"
#	include "BinParser.h"

#	include "Math/angle.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Transformation2D::Transformation2D()
		: m_invalidateWorldMatrix(true)
		, m_invalidateLocalMatrix(true)
		, m_fixedRotation(false)
		, m_origin(0.0f, 0.0f)
		, m_position(0.0f, 0.0f)
		, m_scale(1.0f, 1.0f)
		, m_direction(1.0f, 0.0f)
		, m_angle(0.0f)
	{
		mt::ident_m3( m_localMatrix );
		mt::ident_m3( m_worldMatrix );
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation2D::invalidateWorldMatrix()
	{
		m_invalidateLocalMatrix = true;
		m_invalidateWorldMatrix = true;

		this->_invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation2D::_invalidateWorldMatrix()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation2D::setLocalMatrix( const mt::mat3f & _matrix )
	{
		//assert( 0 );
		//m_position = _matrix.v2.v2;
		m_position.x = _matrix.v2.x;
		m_position.y = _matrix.v2.y;
		
		invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation2D::setLocalPosition( const mt::vec2f & _position )
	{
		m_position = _position;

		invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation2D::setLocalDirection( const mt::vec2f & _direction )
	{
		m_direction = _direction;
		m_angle = mt::signed_angle( _direction );

		invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation2D::setAngle( float _alpha )
	{
		m_angle = _alpha;
		mt::direction( m_direction, _alpha );

		invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation2D::setFixedRotation( bool _fixed )
	{
		m_fixedRotation = _fixed;

		invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation2D::translate( const mt::vec2f & _delta )
	{
		m_position += _delta;

		invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat3f & Transformation2D::updateWorldMatrix( const mt::mat3f & _parentMatrix )
	{
		m_invalidateWorldMatrix = false;

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

		return m_worldMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation2D::loader( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Transformation", "Value", &Transformation2D::setLocalMatrix );
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Transformation", "Position", &Transformation2D::setLocalPosition );
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Transformation", "Direction", &Transformation2D::setLocalDirection );
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Transformation", "Rotate", &Transformation2D::setAngle );
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Transformation", "Angle", &Transformation2D::setAngle );
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Transformation", "Origin", &Transformation2D::setOrigin );
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Transformation", "Scale", &Transformation2D::setScale );
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Transformation", "FixedRotation", &Transformation2D::setFixedRotation );

			XML_CASE_ATTRIBUTE_NODE_METHOD( "Scale", "Value", &Transformation2D::setScale ); //depricated
			XML_CASE_ATTRIBUTE_NODE_METHOD( "FixedRotation", "Value", &Transformation2D::setFixedRotation ); //depricated
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation2D::parser( BinParser * _parser )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Transformation_Value, &Transformation2D::setLocalMatrix );
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Transformation_Position, &Transformation2D::setLocalPosition );
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Transformation_Direction, &Transformation2D::setLocalDirection );
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Transformation_Rotate, &Transformation2D::setAngle );
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Transformation_Angle, &Transformation2D::setAngle );
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Transformation_Origin, &Transformation2D::setOrigin );
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Transformation_Scale, &Transformation2D::setScale );
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Transformation_FixedRotation, &Transformation2D::setFixedRotation );

			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Scale_Value, &Transformation2D::setScale ); //depricated
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::FixedRotation_Value, &Transformation2D::setFixedRotation ); //depricated
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation2D::updateLocalMatrix_()
	{
		m_invalidateLocalMatrix = false;

		mt::mat3f mat_scale;
		mt::ident_m3( mat_scale );
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
		m_localMatrix.v2.x += m_position.x;
		m_localMatrix.v2.y += m_position.y;
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation2D::setOrigin( const mt::vec2f& _origin )
	{
		m_origin = _origin;
		invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation2D::setScale( const mt::vec2f& _scale )
	{
		m_scale = _scale;
		invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation2D::setLocalPositionInt( const mt::vec2f& _position )
	{
		mt::vec2f pos( _position );
		pos.x = ::floorf( pos.x + 0.5f );
		pos.y = ::floorf( pos.y + 0.5f );

		setLocalPosition( pos );
	}
	//////////////////////////////////////////////////////////////////////////
}
