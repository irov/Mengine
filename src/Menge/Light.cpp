#	include "Light.h"
#	include "ObjectImplement.h"

#	include "ScriptEngine.h"
#	include "RenderEngine.h"

#	include "XmlEngine.h"

#	include "SceneNode3D.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Light::Light( const std::string & _name )
		: m_name(_name)
		, m_diffuseColor( 1.0f, 1.0f, 1.0f, 1.0f )
		, m_position( 0.f, 0.f, 0.f )
		, m_parent(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	SceneNode3D * Light::getParentNode() const
	{
		return m_parent;
	}
	//////////////////////////////////////////////////////////////////////////
	void Light::setParentNode( SceneNode3D * _node ) 
	{
		m_parent = _node;
	}
	//////////////////////////////////////////////////////////////////////////
	void Light::loader( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "Color", "Value", m_diffuseColor );
			XML_CASE_ATTRIBUTE_NODE( "Position", "Value", m_position );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Light::activate()
	{
		m_interface = Holder<RenderEngine>::hostage()->createLight( m_name );

		this->getParentNode()->attachLight(this);

		m_interface->setDiffuseColour(m_diffuseColor.r,m_diffuseColor.g,m_diffuseColor.b);
		m_interface->setPosition(m_position.x,m_position.y,m_position.z);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Light::deactivate()
	{
		Holder<RenderEngine>::hostage()->releaseLight( m_interface );
	}
	//////////////////////////////////////////////////////////////////////////
	void Light::setAttenuation( float _range, float _constant, float _linear, float _quadratic )
	{
		return m_interface->setAttenuation(_range, _constant,_linear,_quadratic);
	}
	//////////////////////////////////////////////////////////////////////////
	float Light::getAttenuationRange() const
	{
		return m_interface->getAttenuationRange();
	}
	//////////////////////////////////////////////////////////////////////////
	float Light::getAttenuationConstFactor() const
	{
		return m_interface->getAttenuationConstFactor();
	}
	//////////////////////////////////////////////////////////////////////////
	float Light::getAttenuationLinearFactor() const
	{
		return m_interface->getAttenuationLinearFactor();
	}
	//////////////////////////////////////////////////////////////////////////
	float Light::getAttenuationQuadraticFactor() const
	{
		return m_interface->getAttenuationQuadraticFactor();
	}
	//////////////////////////////////////////////////////////////////////////
	void Light::setSpotlightRange( float _innerAngle, float _outerAngle, float _falloff )
	{
		return m_interface->setSpotlightRange(_innerAngle,_outerAngle,_falloff);
	}
	//////////////////////////////////////////////////////////////////////////
	float Light::getSpotlightInnerAngle() const
	{
		return m_interface->getSpotlightInnerAngle();
	}
	//////////////////////////////////////////////////////////////////////////
	float Light::getSpotlightOuterAngle() const
	{
		return m_interface->getSpotlightOuterAngle();
	}
	//////////////////////////////////////////////////////////////////////////
	float Light::getSpotlightFalloff() const
	{
		return m_interface->getSpotlightFalloff();
	}
	//////////////////////////////////////////////////////////////////////////
	void Light::setDiffuseColour( float _r, float _g, float _b )
	{
		return m_interface->setDiffuseColour(_r,_g,_b);
	}
	//////////////////////////////////////////////////////////////////////////
	void Light::setSpecularColour( float _r, float _g, float _b )
	{
		return m_interface->setSpecularColour(_r,_g,_b);
	}
	//////////////////////////////////////////////////////////////////////////
	bool Light::isEnabled() const
	{
		return m_interface->isEnabled();
	}
	//////////////////////////////////////////////////////////////////////////
	void Light::setEnabled( bool _enabled )
	{
		return m_interface->setEnabled(_enabled);
	}
	//////////////////////////////////////////////////////////////////////////
	void Light::setCastsShadows( bool _enabled )
	{
		return m_interface->setCastsShadows(_enabled);
	}
	//////////////////////////////////////////////////////////////////////////
	void Light::setDirection( float _x, float _y, float _z )
	{
		return m_interface->setDirection(_x,_y,_z);
	}
	//////////////////////////////////////////////////////////////////////////
	void Light::setPosition( float _x, float _y, float _z )
	{
		return m_interface->setPosition(_x,_y,_z);
	}
	//////////////////////////////////////////////////////////////////////////
}