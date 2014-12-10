#	include "Kernel/Transformation3D.h"

#	include "Math/angle.h"

#	include "Core/Exception.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Transformation3D::Transformation3D()
		: m_relationTransformation(nullptr)
		, m_origin(0.f, 0.f, 0.f)
		, m_coordinate(0.f, 0.f, 0.f)
		, m_position(0.f, 0.f, 0.f)
		, m_scale(1.f, 1.f, 1.f)
		, m_orientation(0.f, 0.f, 0.f)
		, m_identityLocalMatrix(true)
		, m_identityWorldMatrix(true)
        , m_invalidateWorldMatrix(true)
        , m_invalidateLocalMatrix(true)
	{
		mt::ident_m4( m_localMatrix );
		mt::ident_m4( m_worldMatrix );
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setRelationTransformation( Transformation3D * _relation )
	{
        if( m_relationTransformation != nullptr )
        {
            m_relationTransformation->removeRelationChildren_( this );
        }

		m_relationTransformation = _relation;

        if( m_relationTransformation != nullptr )
        {
            m_relationTransformation->addRelationChildren_( this );
        }

		this->invalidateWorldMatrix();
	}
    //////////////////////////////////////////////////////////////////////////
    void Transformation3D::addRelationChildren_( Transformation3D * _child )
    {
        m_relationChild.push_back( _child );
    }
    //////////////////////////////////////////////////////////////////////////
    void Transformation3D::removeRelationChildren_( Transformation3D * _child )
    {
        m_relationChild.remove( _child );
    }
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::invalidateWorldMatrix()
	{
		m_invalidateWorldMatrix = true;

		Transformation3D * single = m_relationChild.single();

		if( single != nullptr )
		{
			single->invalidateWorldMatrix();
		}
		else
		{
			for( TSlugTransformation3D it(m_relationChild); it.eof() == false; it.next_shuffle() )
			{
				Transformation3D * transform = *it;
				
				transform->invalidateWorldMatrix();
			}
		}

		this->_invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::_invalidateWorldMatrix()
	{
		//Empty
	}
    //////////////////////////////////////////////////////////////////////////
    void Transformation3D::invalidateLocalMatrix()
    {
        m_invalidateLocalMatrix = true;

        this->invalidateWorldMatrix();
    }
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setLocalPosition( const mt::vec3f & _position )
	{
		if( mt::cmp_v3_v3( m_position, _position ) == true )
		{
			return;
		}

		m_position = _position;

		this->invalidateLocalMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setOrientationX( float _angle )
	{
		if( mt::cmp_f_f( m_orientation.x, _angle ) == true )
		{
			return;
		}

		m_orientation.x = _angle;

		this->invalidateLocalMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setOrientationY( float _angle )
	{
		if( mt::cmp_f_f( m_orientation.y, _angle ) == true )
		{
			return;
		}

		m_orientation.y = _angle;

		this->invalidateLocalMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setOrientationZ( float _angle )
	{
		if( mt::cmp_f_f( m_orientation.z, _angle ) == true )
		{
			return;
		}

		m_orientation.z = _angle;

		this->invalidateLocalMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setOrientation( const mt::vec3f & _euler )
	{
		if( mt::cmp_v3_v3( m_orientation, _euler ) == true )
		{
			return;
		}

		m_orientation = _euler;

        this->invalidateLocalMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setTransformation( const mt::vec3f & _position, const mt::vec3f& _origin, const mt::vec3f & _coordinate, const mt::vec3f& _scale, const mt::vec3f& _orientation )
	{
		m_position = _position;
		m_origin = _origin;
        m_coordinate = _coordinate;
		m_scale = _scale;
		m_orientation = _orientation;

		this->invalidateLocalMatrix();
	}
    //////////////////////////////////////////////////////////////////////////
    void Transformation3D::getTransformation( mt::vec3f & _position, mt::vec3f& _origin, mt::vec3f & _coordinate, mt::vec3f& _scale, mt::vec3f& _orientation )
    {
        _position = m_position;
        _origin = m_origin;
        _coordinate = m_coordinate;
        _scale = m_scale;
        _orientation = m_orientation;
    }
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::resetTransformation()
	{
		m_origin = mt::vec3f(0.f, 0.f, 0.f);
		m_coordinate = mt::vec3f(0.f, 0.f, 0.f);
		m_position = mt::vec3f(0.f, 0.f, 0.f);
		m_scale = mt::vec3f(1.f, 1.f, 1.f);
		m_orientation = mt::vec3f(0.f, 0.f, 0.f);

		this->invalidateLocalMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::translate( const mt::vec3f & _delta )
	{
		mt::vec3f new_pos = m_position + _delta;
		
		this->setLocalPosition( new_pos );
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::lookAt( const mt::vec3f & _position, const mt::vec3f & _at, const mt::vec3f & _up )
	{
		mt::vec3f dir = _at - _position;

		mt::mat4f mr;
		mt::make_rotate_m4_direction( mr, dir, _up );

		mt::vec3f orientation;
		mt::make_euler_angles( orientation, mr );

		this->setLocalPosition( _position );
		this->setOrientation( orientation );
	}
	//////////////////////////////////////////////////////////////////////////
	inline static bool s_identityTransformationMatrix( const mt::mat4f & _m )
	{
		float ident_e = _m.v0.x + _m.v1.y + _m.v2.z + _m.v3.x + _m.v3.y + _m.v3.z;

		if( mt::cmp_f_f( ident_e, 3.f ) == false )
		{
			return false;
		}

		return mt::cmp_f_f( _m.v0.x, 1.f ) == true &&
			mt::cmp_f_f( _m.v1.y, 1.f ) == true &&
			mt::cmp_f_f( _m.v2.z, 1.f ) == true &&
			mt::cmp_f_z( _m.v3.x ) == true &&
			mt::cmp_f_z( _m.v3.y ) == true &&
			mt::cmp_f_z( _m.v3.z ) == true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::updateLocalMatrix() const
	{
		m_invalidateLocalMatrix = false;

#	ifdef _DEBUG
		if( mt::cmp_f_z( m_scale.x ) == true || mt::cmp_f_z( m_scale.y ) == true || mt::cmp_f_z( m_scale.z ) == true )
		{
			MENGINE_THROW_EXCEPTION("Transformation has zero scale [%f, %f, %f]"
				, m_scale.x
				, m_scale.y
				, m_scale.z
				);
		}
#	endif

		mt::mat4f mat_scale;
		mt::ident_m4( mat_scale );
		mat_scale.v3.x = -(m_origin.x + m_coordinate.x) * m_scale.x;
		mat_scale.v3.y = -(m_origin.y + m_coordinate.y) * m_scale.y;
		mat_scale.v3.z = -(m_origin.z + m_coordinate.z) * m_scale.z;
		mat_scale.v0.x = m_scale.x;
		mat_scale.v1.y = m_scale.y;
		mat_scale.v2.z = m_scale.z;
				
		if( mt::cmp_f_z( m_orientation.y ) == true &&
			mt::cmp_f_z( m_orientation.z ) == true )
		{
			if( mt::cmp_f_z( m_orientation.x ) == true )
			{
				m_localMatrix = mat_scale;
			}
			else
			{
				mt::mat4f mat_rot;
				mt::make_rotate_z_axis_m4( mat_rot, m_orientation.x );

				mt::mul_m4_m4( m_localMatrix, mat_scale, mat_rot );
			}
		}
		else
		{
			mt::mat4f mat_rot;
			mt::make_rotate_m4_euler( mat_rot, m_orientation.x, m_orientation.y, m_orientation.z );
        
			mt::mul_m4_m4( m_localMatrix, mat_scale, mat_rot );
		}

		m_localMatrix.v3.x += m_position.x + m_coordinate.x;
		m_localMatrix.v3.y += m_position.y + m_coordinate.y;
		m_localMatrix.v3.z += m_position.z + m_coordinate.z;

		m_identityLocalMatrix = s_identityTransformationMatrix( m_localMatrix );
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::updateWorldMatrix() const
	{
		m_invalidateWorldMatrix = false;

		const mt::mat4f & localMatrix = this->getLocalMatrix();

		if( m_relationTransformation == nullptr )
		{
			m_worldMatrix = localMatrix;

			m_identityWorldMatrix = m_identityLocalMatrix;
		}
		else
		{
			bool identityWorldMatrix = m_relationTransformation->isIdentityWorldMatrix();

			if( identityWorldMatrix == false )
			{
				const mt::mat4f & relationMatrix = m_relationTransformation->getWorldMatrix();

				if( m_identityLocalMatrix == false )
				{				
					mt::mul_m4_m4( m_worldMatrix, localMatrix, relationMatrix );
				}
				else
				{
					m_worldMatrix = relationMatrix;
				}

				m_identityWorldMatrix = false;
			}
			else
			{
				m_worldMatrix = localMatrix;

				m_identityWorldMatrix = m_identityLocalMatrix;
			}
		}
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

		this->invalidateLocalMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setCoordinate( const mt::vec3f& _coordinate )
	{
		if( mt::cmp_v3_v3( m_coordinate, _coordinate ) == true )
		{
			return;
		}

		m_coordinate = _coordinate;

		this->invalidateLocalMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setScale( const mt::vec3f& _scale )
	{
		if( mt::cmp_v3_v3( m_scale, _scale ) == true )
		{
			return;
		}

		m_scale = _scale;
		
		this->invalidateLocalMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
}
