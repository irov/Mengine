#	pragma once

#	include "OgreRenderSystemInterface.h"

#	include "Ogre.h"

class OgreLight : public LightInterface
{
public:
	OgreLight(Ogre::Light * _light);
public:
	void setPosition( float _x, float _y, float _z ) override;
	void setDiffuseColor( float _r, float _g, float _b ) override;
	void setSpecularColour( float _r, float _g, float _b ) override;
	void setAttenuation(float _range, float _constant, float _linear, float _quadratic) override;
	void setDirection(float _x, float _y, float _z) override;
private:
	Ogre::Light * m_light;
};