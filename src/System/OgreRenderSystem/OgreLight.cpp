#	include "OgreLight.h"
//////////////////////////////////////////////////////////////////////////
OgreLight::OgreLight(Ogre::Light * _light)
: m_light(_light)
{
}
//////////////////////////////////////////////////////////////////////////
Ogre::Light * OgreLight::getOgreLight()
{
	return m_light;
}
//////////////////////////////////////////////////////////////////////////
Ogre::Light::LightTypes menge2Ogre( const LightInterface::LightType _type )
{
	switch (_type)
	{
	case LightInterface::LT_POINT: 
		return Ogre::Light::LT_POINT;

	case LightInterface::LT_SPOT: 
		return Ogre::Light::LT_SPOTLIGHT;

	case LightInterface::LT_DIRECTIONAL: 
		return Ogre::Light::LT_DIRECTIONAL;

	default:
		return Ogre::Light::LT_POINT;
	}
}
//////////////////////////////////////////////////////////////////////////
LightInterface::LightType ogre2Menge( const Ogre::Light::LightTypes _type )
{
	switch ( _type )
	{
	case Ogre::Light::LT_POINT: 
		return LightInterface::LT_POINT;

	case Ogre::Light::LT_SPOTLIGHT: 
		return LightInterface::LT_SPOT;

	case Ogre::Light::LT_DIRECTIONAL: 
		return LightInterface::LT_DIRECTIONAL;
	
	default:
		return LightInterface::LT_POINT;
	}
}
//////////////////////////////////////////////////////////////////////////
void OgreLight::setType( const LightType _type )
{
	m_light->setType( menge2Ogre( _type ) );
}
//////////////////////////////////////////////////////////////////////////
LightInterface::LightType OgreLight::getType() const
{
	return ogre2Menge( m_light->getType() );
}
//////////////////////////////////////////////////////////////////////////
void OgreLight::setAttenuation( float _range, float _constant, float _linear, float _quadratic )
{
	m_light->setAttenuation( _range, _constant, _linear, _quadratic );
}
//////////////////////////////////////////////////////////////////////////
float OgreLight::getAttenuationRange() const
{
	return m_light->getAttenuationRange();
}
//////////////////////////////////////////////////////////////////////////
float OgreLight::getAttenuationConstFactor() const
{
	return m_light->getAttenuationConstant();
}
//////////////////////////////////////////////////////////////////////////
float OgreLight::getAttenuationLinearFactor() const
{
	return m_light->getAttenuationLinear();
}
//////////////////////////////////////////////////////////////////////////
float OgreLight::getAttenuationQuadraticFactor() const
{
	return m_light->getAttenuationQuadric();
}
//////////////////////////////////////////////////////////////////////////
void OgreLight::setSpotlightRange( float innerAngle, float outerAngle, float falloff )
{
	m_light->setSpotlightRange( Ogre::Radian(innerAngle), Ogre::Radian(outerAngle), falloff );
}
//////////////////////////////////////////////////////////////////////////
float OgreLight::getSpotlightInnerAngle() const
{
	return m_light->getSpotlightInnerAngle().valueRadians();
}
//////////////////////////////////////////////////////////////////////////
float OgreLight::getSpotlightOuterAngle() const
{
	return m_light->getSpotlightOuterAngle().valueRadians();
}
//////////////////////////////////////////////////////////////////////////
float OgreLight::getSpotlightFalloff() const
{
	return m_light->getSpotlightFalloff();
}
//////////////////////////////////////////////////////////////////////////
void OgreLight::setDiffuseColour( float _r, float _g, float _b )
{
	m_light->setDiffuseColour( _r, _g, _b );
}
//////////////////////////////////////////////////////////////////////////
void OgreLight::setSpecularColour( float _r, float _g, float _b )
{
	m_light->setSpecularColour( _r, _g, _b );
}
//////////////////////////////////////////////////////////////////////////
bool OgreLight::isEnabled() const
{
	return m_light->isVisible();
}
//////////////////////////////////////////////////////////////////////////
void OgreLight::setEnabled( bool _enabled )
{
	m_light->setVisible( _enabled );
}
//////////////////////////////////////////////////////////////////////////
void OgreLight::setCastsShadows( bool _enabled )
{
	m_light->setCastShadows( _enabled );
}
//////////////////////////////////////////////////////////////////////////
void OgreLight::setDirection( float _x, float _y, float _z )
{
	m_light->setDirection( _x, _y, _z );
}
//////////////////////////////////////////////////////////////////////////
void OgreLight::setPosition( float _x, float _y, float _z )
{
	m_light->setPosition(_x,_y,_z);
}
//////////////////////////////////////////////////////////////////////////