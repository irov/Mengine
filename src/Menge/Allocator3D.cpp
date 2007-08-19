#	include "Allocator3D.h"

#	include "XmlParser/XmlParser.h"

#	include "RenderEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Allocator3D::Allocator3D()
		: m_changePivot(false)
		, m_fixedUp( 0.0, 0.f, 1.f )
	{
		mt::ident_m4(m_localMatrix);
		mt::ident_m4(m_worldMatrix);
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D::changePivot()
	{
		_changePivot();
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D::_changePivot()
	{
		m_changePivot = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Allocator3D::isChangePivot()const
	{
		return m_changePivot;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & Allocator3D::getWorldPosition()
	{
		const mt::mat4f &wm = getWorldMatrix();

		return wm.v3.v3;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & Allocator3D::getWorldDirection()
	{
		const mt::mat4f &wm = getWorldMatrix();

		return wm.v2.v3;
	}
	const mt::vec3f & Allocator3D::getWorldStrafe()
	{
		const mt::mat4f &wm = getWorldMatrix();

		return wm.v0.v3;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f & Allocator3D::getWorldMatrix()
	{
		return m_worldMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & Allocator3D::getLocalPosition()const
	{
		return m_localMatrix.v3.v3;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec3f & Allocator3D::getLocalPosition()
	{
		return m_localMatrix.v3.v3;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & Allocator3D::getLocalDirection()const
	{
		return m_localMatrix.v2.v3;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec3f & Allocator3D::getLocalDirection()
	{
		return m_localMatrix.v2.v3;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & Allocator3D::getLocalStrafe()const
	{
		return m_localMatrix.v0.v3;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec3f & Allocator3D::getLocalStrafe()
	{
		return m_localMatrix.v0.v3;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f & Allocator3D::getLocalMatrix()const
	{
		return m_localMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::mat4f & Allocator3D::getLocalMatrix()
	{
		return m_localMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D::setPosition( const mt::vec3f &position )
	{
		mt::vec3f & localPosition = getLocalPosition();

		localPosition = position;

		changePivot();
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D::setDirection( const mt::vec3f &direction )
	{
		mt::norm_v3( m_localMatrix.v3_2, direction );

		mt::cross_v3_v3_norm( m_localMatrix.v3_0, m_fixedUp, m_localMatrix.v3_2 );
		mt::cross_v3_v3_norm( m_localMatrix.v3_1, m_localMatrix.v3_2, m_localMatrix.v3_0 );

		changePivot();
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D::translate( const mt::vec3f &delta )
	{
		m_localMatrix.v3.v3 += delta;

		changePivot();
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D::updateMatrix( Allocator3D * _parent )
	{
		if( m_changePivot == false )
		{
			return;
		}

		_updateMatrix( _parent );

		const mt::mat4f & parentMatrix =
			_parent->getWorldMatrix();

		mt::mul_m4_m4(m_worldMatrix,m_localMatrix,parentMatrix);		

		m_changePivot = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D::_updateMatrix( Allocator3D * _parent )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D::loader( TiXmlElement * _xml )
	{
		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_NODE("Transformation")
			{
				XML_VALUE_ATTRIBUTE("Value", m_localMatrix);

				changePivot();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D::debugRender()
	{
		mt::vec3f orig = m_localMatrix.v3.v3;

		float axisWidth = 50;

		RenderEngine * renderEng = Holder<RenderEngine>::hostage();

		mt::vec3f axis1 = orig + m_localMatrix.v0.v3 * axisWidth;
		mt::vec3f axis2 = orig + m_localMatrix.v1.v3 * axisWidth;
		mt::vec3f axis3 = orig + m_localMatrix.v2.v3 * axisWidth;
		
		renderEng->drawLine3D(orig, axis1, 0xFFFFFFAA);
		renderEng->drawLine3D(orig, axis2, 0xFFAAFFAA);
		renderEng->drawLine3D(orig, axis3, 0xAAFFFFAA);
	}
	//////////////////////////////////////////////////////////////////////////
}