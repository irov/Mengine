#	include "Kernel/Transformation3D.h"

#	include "Math/angle.h"

#	include "Core/Exception.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Transformation3D::Transformation3D()
		: m_relationTransformation(nullptr)
		, m_origin(0.f, 0.f, 0.f)
		, m_position(0.f, 0.f, 0.f)
		, m_scale(1.f, 1.f, 1.f)
		, m_orientation(0.f, 0.f, 0.f)
		, m_identityLocalMatrix(true)
		, m_identityWorldMatrix(true)
        , m_invalidateWorldMatrix(false)
        , m_invalidateLocalMatrix(false)
	{
		mt::ident_m4( m_localMatrix );
		mt::ident_m4( m_worldMatrix );
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setRelationTransformation( Transformation3D * _relation )
	{
		bool identityPrevTransformation = true;

        if( m_relationTransformation != nullptr )
        {
			identityPrevTransformation = m_relationTransformation->isIdentityWorldMatrix();

            m_relationTransformation->removeRelationChildren_( this );
        }

		m_relationTransformation = _relation;

        if( m_relationTransformation != nullptr )
        {
            m_relationTransformation->addRelationChildren_( this );

			if( m_relationTransformation->isIdentityWorldMatrix() == true && m_identityWorldMatrix == true )
			{
				return;
			}
        }
		else
		{
			if( identityPrevTransformation == true )
			{
				return;
			}
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
		if( m_invalidateWorldMatrix == false )
		{
			m_invalidateWorldMatrix = true;

			Transformation3D * single = m_relationChild.single();

			if( single != nullptr )
			{
				single->invalidateWorldMatrix();
			}
			else
			{
				for( TSlugTransformation3D it( m_relationChild ); it.eof() == false; )
				{
					Transformation3D * transform = *it;

					it.next_shuffle();

					transform->invalidateWorldMatrix();
				}
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
	void Transformation3D::setLocalPositionX( float _x )
	{
		if( mt::equal_f_f( m_position.x, _x ) == true )
		{
			return;
		}

		m_position.x = _x;

		this->invalidateLocalMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	float Transformation3D::getLocalPositionX() const
	{
		return m_position.x;
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setLocalPositionY( float _y )
	{
		if( mt::equal_f_f( m_position.y, _y ) == true )
		{
			return;
		}

		m_position.y = _y;

		this->invalidateLocalMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	float Transformation3D::getLocalPositionY() const
	{
		return m_position.y;
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setLocalPositionZ( float _z )
	{
		if( mt::equal_f_f( m_position.z, _z ) == true )
		{
			return;
		}

		m_position.z = _z;

		this->invalidateLocalMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	float Transformation3D::getLocalPositionZ() const
	{
		return m_position.z;
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setOrientationX( float _angle )
	{
		if( mt::equal_f_f( m_orientation.x, _angle ) == true )
		{
			return;
		}

		m_orientation.x = _angle;

		this->invalidateLocalMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setOrientationY( float _angle )
	{
		if( mt::equal_f_f( m_orientation.y, _angle ) == true )
		{
			return;
		}

		m_orientation.y = _angle;

		this->invalidateLocalMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setOrientationZ( float _angle )
	{
		if( mt::equal_f_f( m_orientation.z, _angle ) == true )
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
	void Transformation3D::setTransformation( const mt::vec3f & _position, const mt::vec3f& _origin, const mt::vec3f& _scale, const mt::vec3f& _orientation )
	{
		m_position = _position;
		m_origin = _origin;
		m_scale = _scale;
		m_orientation = _orientation;

		this->invalidateLocalMatrix();
	}
    //////////////////////////////////////////////////////////////////////////
    void Transformation3D::getTransformation( mt::vec3f & _position, mt::vec3f& _origin, mt::vec3f& _scale, mt::vec3f& _orientation ) const
    {
        _position = m_position;
        _origin = m_origin;
		_scale = m_scale;
        _orientation = m_orientation;
    }
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::calcWorldMatrix( mt::mat4f & _wm, const mt::vec3f & _position, const mt::vec3f& _origin, const mt::vec3f& _scale, const mt::vec3f& _orientation ) const
	{
		mt::mat4f localMatrix;
		bool identityLocalMatrix = Transformation3D::makeLocalMatrix_( localMatrix, _position, _origin, _scale, _orientation );

		if( m_relationTransformation == nullptr )
		{
			_wm = localMatrix;
		}
		else
		{
			bool identityWorldMatrix = m_relationTransformation->isIdentityWorldMatrix();

			if( identityWorldMatrix == false )
			{
				const mt::mat4f & relationMatrix = m_relationTransformation->getWorldMatrix();

				if( identityLocalMatrix == false )
				{
					mt::mul_m4_m4( _wm, localMatrix, relationMatrix );
				}
				else
				{
					_wm = relationMatrix;
				}
			}
			else
			{
				if( identityLocalMatrix == false )
				{
					_wm = localMatrix;
				}
				else
				{
					mt::ident_m4( _wm );
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::resetTransformation()
	{
		m_origin = mt::vec3f(0.f, 0.f, 0.f);
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
	void Transformation3D::coordinate( const mt::vec3f & _delta )
	{
		mt::vec3f new_pos = m_position + _delta;
		mt::vec3f new_origin = m_origin + _delta;

		this->setLocalPosition( new_pos );
		this->setOrigin( new_origin );
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setDirection( const mt::vec3f & _direction, const mt::vec3f & _up )
	{
		mt::mat4f mr;
		mt::make_rotate_m4_direction( mr, _direction, _up );

		mt::vec3f orientation;
		mt::make_euler_angles( orientation, mr );

		this->setOrientation( orientation );
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setBillboard( const mt::vec3f & _direction, const mt::vec3f & _normal )
	{
		mt::mat4f mr;
		mt::make_rotate_m4_fixed_up( mr, _direction, _normal );

		mt::vec3f orientation;
		mt::make_euler_angles( orientation, mr );

		this->setOrientation( orientation );
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::billboardAt( const mt::vec3f & _position, const mt::vec3f & _at, const mt::vec3f & _normal )
	{
		mt::vec3f dir = _at - _position;

		mt::mat4f mr;
		mt::make_rotate_m4_fixed_up( mr, dir, _normal );

		mt::vec3f orientation;
		mt::make_euler_angles( orientation, mr );

		this->setOrientation( orientation );

		this->setLocalPosition( _position );
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::setAxes( const mt::vec3f & _direction, const mt::vec3f & _left, const mt::vec3f & _up )
	{
		mt::mat4f mr;
		mt::make_rotate_m4_axes( mr, _direction, _left, _up );

		mt::vec3f orientation;
		mt::make_euler_angles( orientation, mr );

		this->setOrientation( orientation );
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::lookAt( const mt::vec3f & _position, const mt::vec3f & _at, const mt::vec3f & _up )
	{
		mt::vec3f dir = _at - _position;

		mt::mat4f mr;
		mt::make_rotate_m4_direction( mr, dir, _up );

		mt::vec3f orientation;
		mt::make_euler_angles( orientation, mr );

		this->setOrientation( orientation );

		this->setLocalPosition( _position );
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec3f Transformation3D::getAxisDirection() const
	{
		mt::mat4f mat_rot;
		mt::make_rotate_m4_euler( mat_rot, m_orientation.x, m_orientation.y, m_orientation.z );

		mt::vec3f axis = mat_rot.v0.to_vec3f();

		return axis;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec3f Transformation3D::getAxisLeft() const
	{
		mt::mat4f mat_rot;
		mt::make_rotate_m4_euler( mat_rot, m_orientation.x, m_orientation.y, m_orientation.z );

		mt::vec3f axis = mat_rot.v1.to_vec3f();

		return axis;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec3f Transformation3D::getAxisUp() const
	{
		mt::mat4f mat_rot;
		mt::make_rotate_m4_euler( mat_rot, m_orientation.x, m_orientation.y, m_orientation.z );

		mt::vec3f axis = mat_rot.v2.to_vec3f();

		return axis;
	}
	//////////////////////////////////////////////////////////////////////////
	inline static bool s_identityTransformationMatrix( const mt::mat4f & _m )
	{
		float ident_e = _m.v0.x + _m.v1.y + _m.v2.z + _m.v3.x + _m.v3.y + _m.v3.z;

		if( mt::equal_f_f( ident_e, 3.f ) == false )
		{
			return false;
		}

		return mt::equal_f_f( _m.v0.x, 1.f ) == true &&
			mt::equal_f_f( _m.v1.y, 1.f ) == true &&
			mt::equal_f_f( _m.v2.z, 1.f ) == true &&
			mt::equal_f_z( _m.v3.x ) == true &&
			mt::equal_f_z( _m.v3.y ) == true &&
			mt::equal_f_z( _m.v3.z ) == true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Transformation3D::updateLocalMatrix() const
	{
		m_invalidateLocalMatrix = false;
		
		bool identityLocalMatrix = this->makeLocalMatrix_( m_localMatrix, m_position, m_origin, m_scale, m_orientation );

		m_identityLocalMatrix = identityLocalMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Transformation3D::makeLocalMatrix_( mt::mat4f & _lm, const mt::vec3f & _position, const mt::vec3f& _origin, const mt::vec3f& _scale, const mt::vec3f& _orientation )
	{
		mt::mat4f mat_scale;
		mt::ident_m4( mat_scale );
		mat_scale.v3.x = -_origin.x * _scale.x;
		mat_scale.v3.y = -_origin.y * _scale.y;
		mat_scale.v3.z = -_origin.z * _scale.z;
		mat_scale.v0.x = _scale.x;
		mat_scale.v1.y = _scale.y;
		mat_scale.v2.z = _scale.z;

		if( mt::equal_f_z( _orientation.y ) == true &&
			mt::equal_f_z( _orientation.z ) == true )
		{
			if( mt::equal_f_z( _orientation.x ) == true )
			{
				_lm = mat_scale;
			}
			else
			{
				mt::mat4f mat_rot;
				mt::make_rotate_z_axis_m4( mat_rot, _orientation.x );

				mt::mul_m4_m4( _lm, mat_scale, mat_rot );
			}
		}
		else
		{
			mt::mat4f mat_rot;
			mt::make_rotate_m4_euler( mat_rot, _orientation.x, _orientation.y, _orientation.z );

			mt::mul_m4_m4( _lm, mat_scale, mat_rot );
		}

		_lm.v3.x += _position.x;
		_lm.v3.y += _position.y;
		_lm.v3.z += _position.z;

		bool identityLocalMatrix = s_identityTransformationMatrix( _lm );

		return identityLocalMatrix;
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
				if( m_identityWorldMatrix == false || m_identityLocalMatrix == false )
				{
					m_worldMatrix = localMatrix;

					m_identityWorldMatrix = m_identityLocalMatrix;
				}
				else
				{
					mt::ident_m4( m_worldMatrix );

					m_identityWorldMatrix = true;
				}
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
	const mt::vec3f & Transformation3D::getWorldPosition() const
	{
		const mt::mat4f & wm = this->getWorldMatrix();

		const mt::vec3f & wp = wm.v3.to_vec3f();

		return wp;
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
