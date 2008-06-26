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
	bool Light2D::isVisible( const Viewport & _viewPort )
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Light2D::loader( XmlElement * _xml )
	{
		SceneNode2D::loader(_xml);

		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE( "PenumbraRadius", "Value", m_shadowPenumbraR );
			XML_CASE_ATTRIBUTE_NODE( "ShadowLength", "Value", m_shadowLength );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Light2D::_render( bool _enableDebug )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Light2D::_activate()
	{
		if( SceneNode2D::_activate() == false )
		{
			return false;
		}

		Holder<LightSystem>::hostage()->regLight(this);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Light2D::_deactivate()
	{
		SceneNode2D::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Light2D::_compile()
	{
		if( SceneNode2D::_compile() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Light2D::_release()
	{
		SceneNode2D::_release();
	}
	//////////////////////////////////////////////////////////////////////////
	void Light2D::_update( float _timing )
	{
		SceneNode2D::_update(_timing);
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