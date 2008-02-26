#	pragma once

#	include "math/quat.h"
#	include <string>
#	include "Color.h"

class LightInterface;

class XmlElement;

namespace Menge
{
	class SceneNode3D;

	const int LIGHT_POINT = 0;
	const int LIGHT_DIRECTIONAL = 1;
	const int LIGHT_SPOT = 2;

	class Light
	{
	public:
		Light( const std::string & _name );
	public:
		void setAttenuation( float _range, float _constant, float _linear, float _quadratic );
		float getAttenuationRange() const;
		float getAttenuationConstFactor() const;
		float getAttenuationLinearFactor() const;
		float getAttenuationQuadraticFactor() const;

		void setSpotlightRange( float _innerAngle, float _outerAngle, float _falloff );
		float getSpotlightInnerAngle() const;
		float getSpotlightOuterAngle() const;
		float getSpotlightFalloff() const;

		void setDiffuseColour( float _r, float _g, float _b );
		void setSpecularColour( float _r, float _g, float _b );

		bool isVisible() const;
		void setVisible( bool _enabled );
		void setCastsShadows( bool _enabled );

		void setDirection( float _x, float _y, float _z );
	
		//hack ??
		LightInterface * get()
		{
			return m_interface;
		}

	public:
		void loader( XmlElement * _xml );
		bool activate();
		void deactivate();
		void release();
	public:
		SceneNode3D * getParentNode() const;
		void setParentNode( SceneNode3D * _node );
	private:
		std::string m_name;
		LightInterface * m_interface;
		SceneNode3D * m_parent;

		int m_lightType;
		mt::vec3f	m_position;
        Color m_diffuse;
        Color m_specular;
		mt::vec3f m_direction;
		float m_spotOuter;
        float m_spotInner;
        float m_spotFalloff;
		float m_range;
		float m_attenuationConst;
		float m_attenuationLinear;
        float m_attenuationQuad;
	};
}