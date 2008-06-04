//#	include "Light.h"
//#	include "ObjectImplement.h"
//
//#	include "ScriptEngine.h"
//#	include "RenderEngine.h"
//
//#	include "XmlEngine.h"
//
//#	include "SceneNode3D.h"
//
//namespace	Menge
//{
//	//////////////////////////////////////////////////////////////////////////
//	Light::Light()
//	: m_lightInterface(0)
//	, m_lightType(LT_POINT)
//	, m_position(0.f, 0.f, 0.f)
//    , m_diffuse(1.0f, 1.0f, 1.0f, 1.0f)
//    , m_specular(1.0f, 1.0f, 1.0f, 1.0f)
//    , m_direction(0,0,1)
//	, m_spotOuter(40) 
//    , m_spotInner(30)
//    , m_spotFalloff(1.0f)
//	, m_range(100000)
//	, m_attenuationConst(1.0f)
//	, m_attenuationLinear(0.0f)
//    , m_attenuationQuad(0.0f)
//	{
//	}
//	//////////////////////////////////////////////////////////////////////////
//	Light::~Light()
//	{
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void Light::loader( XmlElement * _xml )
//	{
//		SceneNode3D::loader(_xml);
//
//		XML_SWITCH_NODE( _xml )
//		{
//			XML_CASE_ATTRIBUTE_NODE( "Type", "Value", m_lightType );
//			XML_CASE_ATTRIBUTE_NODE( "Position", "Value", m_position );
//			XML_CASE_ATTRIBUTE_NODE( "Direction", "Value", m_direction );
//			XML_CASE_ATTRIBUTE_NODE( "Diffuse", "Value", m_diffuse );
//			XML_CASE_ATTRIBUTE_NODE( "Specular", "Value", m_specular );
//			XML_CASE_ATTRIBUTE_NODE( "SpotOuter", "Value", m_spotOuter );
//			XML_CASE_ATTRIBUTE_NODE( "SpotInner", "Value", m_spotInner );
//			XML_CASE_ATTRIBUTE_NODE( "SpotFalloff", "Value", m_spotFalloff );
//			XML_CASE_ATTRIBUTE_NODE( "Range", "Value", m_range );
//			XML_CASE_ATTRIBUTE_NODE( "AttenuationConst", "Value", m_attenuationConst );
//			XML_CASE_ATTRIBUTE_NODE( "AttenuationLinear", "Value", m_attenuationLinear );
//			XML_CASE_ATTRIBUTE_NODE( "AttenuationQuad", "Value", m_attenuationQuad );
//		}
//	}
//	//////////////////////////////////////////////////////////////////////////
//	bool Light::_activate()
//	{
//		if( SceneNode3D::_activate() == false )
//		{
//			return false;
//		}
//
//		return true;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void Light::_deactivate()
//	{
//		SceneNode3D::_deactivate();
//	}
//	//////////////////////////////////////////////////////////////////////////
//	bool Light::_compile()
//	{
//		if( SceneNode3D::_compile() == false )
//		{
//			return false;
//		}
//
//		m_lightInterface = Holder<RenderEngine>::hostage()->createLight( m_name );
//
//		m_lightInterface->setType((LightType)m_lightType);
//		m_lightInterface->setPosition(m_position.x,m_position.y,m_position.z);
//		m_lightInterface->setDirection(m_direction.x,m_direction.y,m_direction.z);
//		m_lightInterface->setAttenuation( m_range, m_attenuationConst, m_attenuationLinear, m_attenuationQuad );
//
//		if(m_lightType == LT_SPOT)
//		{
//			m_lightInterface->setSpotlightRange( m_spotInner, m_spotOuter, m_spotFalloff );
//		}
//
//		m_lightInterface->setDiffuseColour(m_diffuse.r,m_diffuse.g,m_diffuse.b);
//		m_lightInterface->setSpecularColour( m_specular.r, m_specular.g, m_specular.b );
//		//m_lightInterface->setCastsShadows( bool _enabled );
//
//		m_interface->attachLight(m_lightInterface);
//
//		return true;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void Light::_update( float _timing )
//	{
//		SceneNode3D::_update(_timing);
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void Light::_release()
//	{
//		SceneNode3D::_release();
//
//		Holder<RenderEngine>::hostage()->releaseLight( m_lightInterface );
//		m_lightInterface = 0;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void Light::setAttenuation( float _range, float _constant, float _linear, float _quadratic )
//	{
//		return m_lightInterface->setAttenuation(_range, _constant,_linear,_quadratic);
//	}
//	//////////////////////////////////////////////////////////////////////////
//	float Light::getAttenuationRange() const
//	{
//		return m_lightInterface->getAttenuationRange();
//	}
//	//////////////////////////////////////////////////////////////////////////
//	float Light::getAttenuationConstFactor() const
//	{
//		return m_lightInterface->getAttenuationConstFactor();
//	}
//	//////////////////////////////////////////////////////////////////////////
//	float Light::getAttenuationLinearFactor() const
//	{
//		return m_lightInterface->getAttenuationLinearFactor();
//	}
//	//////////////////////////////////////////////////////////////////////////
//	float Light::getAttenuationQuadraticFactor() const
//	{
//		return m_lightInterface->getAttenuationQuadraticFactor();
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void Light::setSpotlightRange( float _innerAngle, float _outerAngle, float _falloff )
//	{
//		return m_lightInterface->setSpotlightRange(_innerAngle,_outerAngle,_falloff);
//	}
//	//////////////////////////////////////////////////////////////////////////
//	float Light::getSpotlightInnerAngle() const
//	{
//		return m_lightInterface->getSpotlightInnerAngle();
//	}
//	//////////////////////////////////////////////////////////////////////////
//	float Light::getSpotlightOuterAngle() const
//	{
//		return m_lightInterface->getSpotlightOuterAngle();
//	}
//	//////////////////////////////////////////////////////////////////////////
//	float Light::getSpotlightFalloff() const
//	{
//		return m_lightInterface->getSpotlightFalloff();
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void Light::setDiffuseColour( float _r, float _g, float _b )
//	{
//		return m_lightInterface->setDiffuseColour(_r,_g,_b);
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void Light::setSpecularColour( float _r, float _g, float _b )
//	{
//		return m_lightInterface->setSpecularColour(_r,_g,_b);
//	}
//	//////////////////////////////////////////////////////////////////////////
//	bool Light::isVisible() const
//	{
//		return m_lightInterface->isVisible();
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void Light::setVisible( bool _enabled )
//	{
//		return m_lightInterface->setVisible(_enabled);
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void Light::setCastsShadows( bool _enabled )
//	{
//		return m_lightInterface->setCastsShadows(_enabled);
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void Light::setDirection( float _x, float _y, float _z )
//	{
//		return m_lightInterface->setDirection(_x,_y,_z);
//	}
//	// по идеи через SceneNode, хотя??
//	//////////////////////////////////////////////////////////////////////////
///*	void Light::setPosition( float _x, float _y, float _z )
//	{
//		return m_interface->setPosition(_x,_y,_z);
//	}*/
//	//////////////////////////////////////////////////////////////////////////
//}