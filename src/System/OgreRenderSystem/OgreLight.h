#	pragma once

#	include "OgreRenderSystemInterface.h"

#	include "Ogre.h"

class OgreLight : public LightInterface
{
public:
	OgreLight(Ogre::Light * _light);
public:

	void setType( LightType _type ) override;
	LightType getType() const override;

	void setAttenuation( float _range, float _constant, float _linear, float _quadratic ) override;
	float getAttenuationRange() const override;
	float getAttenuationConstFactor() const override;
	float getAttenuationLinearFactor() const override;
	float getAttenuationQuadraticFactor() const override;

	void setSpotlightRange( float _innerAngle, float _outerAngle, float _falloff ) override;
	float getSpotlightInnerAngle() const override;
	float getSpotlightOuterAngle() const override;
	float getSpotlightFalloff() const override;

	void setDiffuseColour( float _r, float _g, float _b ) override;
	void setSpecularColour( float _r, float _g, float _b ) override;

	bool isEnabled() const override;
	void setEnabled( bool _enabled ) override;
	void setCastsShadows( bool _enabled ) override;

	void setDirection( float _x, float _y, float _z ) override;
	void setPosition( float _x, float _y, float _z ) override;

public:
	Ogre::Light * getOgreLight();

private:
	Ogre::Light * m_light;
};