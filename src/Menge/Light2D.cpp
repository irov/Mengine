#	include "Light2D.h"

#	include "ObjectImplement.h"

#	include "XmlEngine.h"

#	include "LightSystem.h"

#	include "RenderEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(Light2D);
	//////////////////////////////////////////////////////////////////////////
	Light2D::Light2D()
		: m_shadowLength(200)
		, m_shadowPenumbraR(20)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Light2D::loader( XmlElement * _xml )
	{
		Node::loader(_xml);

		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE( "PenumbraRadius", "Value", m_shadowPenumbraR );
			XML_CASE_ATTRIBUTE_NODE( "ShadowLength", "Value", m_shadowLength );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Light2D::_render( const Viewport & _viewport, bool _enableDebug )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Light2D::_activate()
	{
		if( Node::_activate() == false )
		{
			return false;
		}

		Holder<LightSystem>::hostage()->regLight(this);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Light2D::_deactivate()
	{
		Node::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Light2D::_compile()
	{
		if( Node::_compile() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Light2D::_release()
	{
		Node::_release();
	}
	//////////////////////////////////////////////////////////////////////////
	void Light2D::_update( float _timing )
	{
		Node::_update(_timing);
	}
	//////////////////////////////////////////////////////////////////////////
	void Light2D::setShadowLength(float _length)
	{
		m_shadowLength = _length;
	}
	//////////////////////////////////////////////////////////////////////////
	void Light2D::setPenumbraRadius(float _radius)
	{
		m_shadowPenumbraR = _radius;
	}
	//////////////////////////////////////////////////////////////////////////
	float Light2D::getShadowLength() const
	{
		return m_shadowLength;
	}
	//////////////////////////////////////////////////////////////////////////
	float Light2D::getPenumbraRadius() const
	{
		return m_shadowPenumbraR;
	}
}