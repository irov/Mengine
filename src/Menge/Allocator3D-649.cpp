#	include "Allocator3D-649.h"

#	include "XmlEngine.h"

#	include "RenderEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Allocator3D_::Allocator3D_()
		: m_changePivot(false)
		, m_fixedUp( 0.0, 1.f, 0.f )
	{
		mt::ident_m4(m_localMatrix);
		mt::ident_m4(m_worldMatrix);
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D_::changePivot()
	{
		_changePivot();
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D_::_changePivot()
	{
		m_changePivot = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Allocator3D_::isChangePivot()const
	{
		return m_changePivot;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & Allocator3D_::getWorldPosition()
	{
		const mt::mat4f &wm = getWorldMatrix();

		return wm.v3.v3;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & Allocator3D_::getWorldDirection()
	{
		const mt::mat4f &wm = getWorldMatrix();

		return wm.v2.v3;
	}
	const mt::vec3f & Allocator3D_::getWorldStrafe()
	{
		const mt::mat4f &wm = getWorldMatrix();

		return wm.v0.v3;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f & Allocator3D_::getWorldMatrix()
	{
		return m_worldMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & Allocator3D_::getLocalPosition()const
	{
		return m_localMatrix.v3.v3;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec3f & Allocator3D_::getLocalPosition()
	{
		return m_localMatrix.v3.v3;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & Allocator3D_::getLocalDirection()const
	{
		return m_localMatrix.v2.v3;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec3f & Allocator3D_::getLocalDirection()
	{
		return m_localMatrix.v2.v3;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & Allocator3D_::getLocalStrafe()const
	{
		return m_localMatrix.v0.v3;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec3f & Allocator3D_::getLocalStrafe()
	{
		return m_localMatrix.v0.v3;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f & Allocator3D_::getLocalMatrix()const
	{
		return m_localMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::mat4f & Allocator3D_::getLocalMatrix()
	{
		return m_localMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D_::setLocalPosition( const mt::vec3f &position )
	{
		mt::vec3f & localPosition = getLocalPosition();

		localPosition = position;

		changePivot();
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D_::setDirection( const mt::vec3f &direction )
	{
		mt::norm_v3( m_localMatrix.v3_2, direction );

		mt::cross_v3_v3_norm( m_localMatrix.v3_0, m_fixedUp, m_localMatrix.v3_2 );
		mt::cross_v3_v3_norm( m_localMatrix.v3_1, m_localMatrix.v3_2, m_localMatrix.v3_0 );

		changePivot();
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D_::translate( const mt::vec3f &delta )
	{
		m_localMatrix.v3.v3 += delta;

		changePivot();
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D_::updateMatrix( Allocator3D_ * _parent )
	{
		if( m_changePivot == false )
		{
			return;
		}

		if( _parent )
		{
			const mt::mat4f & parentMatrix =
				_parent->getWorldMatrix();

			mt::mul_m4_m4(m_worldMatrix,m_localMatrix,parentMatrix);		
		}

		_updateMatrix( _parent );

		m_changePivot = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D_::_updateMatrix( Allocator3D_ * _parent )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D_::loader( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE("Transformation")
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					//XML_CASE_ATTRIBUTE("Value", m_localMatrix);
					XML_CASE_ATTRIBUTE_MEMBER( "Position", &Allocator3D_::setLocalPosition );
				}

				changePivot();
			}
		}
	}
}