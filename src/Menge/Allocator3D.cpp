#	include "Allocator3D.h"

#	include "XmlEngine.h"

#	include "RenderEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Allocator3D::Allocator3D()
		: m_changePivot3D(false)
		, m_fixedUp( 0.0, 1.f, 0.f )
	{
		mt::ident_m4(m_localMatrix3D);
		mt::ident_m4(m_worldMatrix3D);
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D::changePivot3D()
	{
		_changePivot3D();
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D::_changePivot3D()
	{
		m_changePivot3D = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Allocator3D::isChangePivot3D() const
	{
		return m_changePivot3D;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & Allocator3D::getWorldPosition3D()
	{
		const mt::mat4f &wm = getWorldMatrix3D();

		return mt::cast_v4_v3( wm.v3 );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & Allocator3D::getWorldDirection3D()
	{
		const mt::mat4f &wm = getWorldMatrix3D();

		return mt::cast_v4_v3( wm.v2 );
	}
	const mt::vec3f & Allocator3D::getWorldStrafe3D()
	{
		const mt::mat4f &wm = getWorldMatrix3D();

		return mt::cast_v4_v3( wm.v0 );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f & Allocator3D::getWorldMatrix3D()
	{
		return m_worldMatrix3D;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & Allocator3D::getLocalPosition3D() const
	{
		return mt::cast_v4_v3( m_localMatrix3D.v3 );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & Allocator3D::getLocalDirection3D() const
	{
		return mt::cast_v4_v3( m_localMatrix3D.v2 );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & Allocator3D::getLocalStrafe3D() const
	{
		return mt::cast_v4_v3( m_localMatrix3D.v0 );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f & Allocator3D::getLocalMatrix3D() const
	{
		return m_localMatrix3D;
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D::setLocalPosition3D( const mt::vec3f &position )
	{
		mt::set_v4_v3( m_localMatrix3D.v3, position );

		changePivot3D();
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D::setDirection3D( const mt::vec3f &direction )
	{
		mt::vec3f dir;
		mt::norm_v3( dir, direction );

		mt::set_v4_v3( m_localMatrix3D.v2, dir );

		mt::vec3f vec1, vec2;
		mt::cross_v3_v3_norm( vec1, m_fixedUp, dir );
		mt::cross_v3_v3_norm( vec2, dir, vec1 );

		mt::set_v4_v3( m_localMatrix3D.v0, vec1 );
		mt::set_v4_v3( m_localMatrix3D.v1, vec2 );

		changePivot3D();
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D::translate3D( const mt::vec3f &delta )
	{
		//m_localMatrix3D.v3.v3 += delta;
		add_v4_v3( m_localMatrix3D.v3, m_localMatrix3D.v3, delta );

		changePivot3D();
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D::updateMatrix3D( const mt::mat4f & parentMatrix /*Allocator3D * _parent*/ )
	{
		if( m_changePivot3D == false )
		{
			return;
		}

		//if( _parent )
		{
			//const mt::mat4f & parentMatrix =
			//	_parent->getWorldMatrix3D();

			mt::mul_m4_m4(m_worldMatrix3D,m_localMatrix3D,parentMatrix);		
		}

		_updateMatrix3D();

		m_changePivot3D = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D::_updateMatrix3D()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D::loader( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Transformation3D" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					//XML_CASE_ATTRIBUTE("Value", m_localMatrix);
					XML_CASE_ATTRIBUTE_MEMBER( "Position", &Allocator3D::setLocalPosition3D );
				}

				changePivot3D();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D::scale( const mt::vec3f& _scale )
	{
		m_localMatrix3D.v0.x *= _scale.x;
		m_localMatrix3D.v1.y *= _scale.y;
		m_localMatrix3D.v2.z *= _scale.z;
	}
	//////////////////////////////////////////////////////////////////////////
}