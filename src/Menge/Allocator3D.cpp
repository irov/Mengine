#	include "Allocator3D.h"

#	include "XmlEngine.h"

#	include "RenderEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Allocator3D::Allocator3D()
		: m_changePivot(false)
		, m_localOrient( 1.f, 0.f, 0.f, 0.f )
		, m_worldOrient( 1.f, 0.f, 0.f, 0.f )
		, m_localPosition( 0.f, 0.f, 0.f )
		, m_worldPosition( 0.f, 0.f, 0.f )
	{}
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
	const mt::vec3f & Allocator3D::getWorldPosition()
	{
		return m_worldPosition;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & Allocator3D::getLocalPosition()const
	{
		return m_localPosition;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec3f & Allocator3D::getLocalPosition()
	{
		return m_localPosition;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::quatf & Allocator3D::getLocalOrient()const
	{
		return m_localOrient;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::quatf & Allocator3D::getLocalOrient()
	{
		return m_localOrient;
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D::setLocalPosition( const mt::vec3f &position )
	{
		m_localPosition = position;
		changePivot();
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D::setLocalOrient( const mt::quatf &quat )
	{
		m_localOrient = quat;
		changePivot();
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D::translate( const mt::vec3f &delta )
	{
		m_localPosition += delta;
		changePivot();
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D::updateOrient( Allocator3D * _parent )
	{
	/*	if( m_changePivot == false )
		{
			return;
		}*/

		const mt::quatf & parentOrient =
			_parent->getWorldOrient();

		m_worldOrient = m_localOrient * parentOrient;

		m_changePivot = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D::updatePosition( Allocator3D * _parent )
	{
		if( m_changePivot == false )
		{
			return;
		}

		const mt::vec3f & parentPosition =
			_parent->getWorldPosition();

		m_worldPosition = m_localPosition + parentPosition;

		m_changePivot = false;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::quatf & Allocator3D::getWorldOrient()
	{
		return m_worldOrient;
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
					XML_CASE_ATTRIBUTE("Value", m_localPosition);
				}

				changePivot();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator3D::debugRender()
	{}
	//////////////////////////////////////////////////////////////////////////
}