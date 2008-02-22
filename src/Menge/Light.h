#	pragma once

#	include "math/quat.h"
#	include <string>
#	include "Color.h"

class LightInterface;

class XmlElement;

namespace Menge
{
	class SceneNode3D;

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

		bool isEnabled() const;
		void setEnabled( bool _enabled );
		void setCastsShadows( bool _enabled );

		void setDirection( float _x, float _y, float _z );
		void setPosition( float _x, float _y, float _z );

		//hack ??
		LightInterface * get()
		{
			return m_interface;
		}

	public:
		virtual void loader( XmlElement * _xml );
		virtual bool activate();
		virtual void deactivate();
	public:
		SceneNode3D * getParentNode() const;
		void setParentNode( SceneNode3D * _node );
	private:
		std::string m_name;
		LightInterface * m_interface;
		Color m_diffuseColor;
		mt::vec3f	m_position;
		SceneNode3D * m_parent;
	};
}