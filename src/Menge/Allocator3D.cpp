#	include "Allocator3D.h"

#	include "XmlEngine.h"

#	include "RenderEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Allocator3D::Allocator3D()
		: m_changePivot(false)
		, m_fixedUp( 0.0, 1.f, 0.f )
	{
		mt::ident_m4(m_localMatrix);
		mt::ident_m4(m_worldMatrix);
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D::changePivot3D()
	{
		_changePivot3D();
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D::_changePivot3D()
	{
		m_changePivot = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Allocator3D::isChangePivot3D() const
	{
		return m_changePivot;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & Allocator3D::getWorldPosition3D()
	{
		const mt::mat4f &wm = getWorldMatrix3D();

		return wm.v3.v3;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & Allocator3D::getWorldDirection3D()
	{
		const mt::mat4f &wm = getWorldMatrix3D();

		return wm.v2.v3;
	}
	const mt::vec3f & Allocator3D::getWorldStrafe3D()
	{
		const mt::mat4f &wm = getWorldMatrix3D();

		return wm.v0.v3;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f & Allocator3D::getWorldMatrix3D()
	{
		return m_worldMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & Allocator3D::getLocalPosition3D() const
	{
		return m_localMatrix.v3.v3;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec3f & Allocator3D::getLocalPosition3D()
	{
		return m_localMatrix.v3.v3;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & Allocator3D::getLocalDirection3D() const
	{
		return m_localMatrix.v2.v3;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec3f & Allocator3D::getLocalDirection3D()
	{
		return m_localMatrix.v2.v3;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & Allocator3D::getLocalStrafe3D() const
	{
		return m_localMatrix.v0.v3;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec3f & Allocator3D::getLocalStrafe3D()
	{
		return m_localMatrix.v0.v3;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f & Allocator3D::getLocalMatrix3D() const
	{
		return m_localMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::mat4f & Allocator3D::getLocalMatrix3D()
	{
		return m_localMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D::setLocalPosition3D( const mt::vec3f &position )
	{
		mt::vec3f & localPosition = getLocalPosition3D();

		localPosition = position;

		changePivot3D();
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D::setDirection3D( const mt::vec3f &direction )
	{
		mt::norm_v3( m_localMatrix.v3_2, direction );

		mt::cross_v3_v3_norm( m_localMatrix.v3_0, m_fixedUp, m_localMatrix.v3_2 );
		mt::cross_v3_v3_norm( m_localMatrix.v3_1, m_localMatrix.v3_2, m_localMatrix.v3_0 );

		changePivot3D();
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D::translate3D( const mt::vec3f &delta )
	{
		m_localMatrix.v3.v3 += delta;

		changePivot3D();
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D::updateMatrix3D( Allocator3D * _parent )
	{
		if( m_changePivot == false )
		{
			return;
		}

		if( _parent )
		{
			const mt::mat4f & parentMatrix =
				_parent->getWorldMatrix3D();

			mt::mul_m4_m4(m_worldMatrix,m_localMatrix,parentMatrix);		
		}

		_updateMatrix3D( _parent );

		m_changePivot = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D::_updateMatrix3D( Allocator3D * _parent )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D::loader( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE("Transformation")
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
}