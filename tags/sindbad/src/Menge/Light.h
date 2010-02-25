#	pragma once

#	include "math/quat.h"
#	include <string>
#	include "Color.h"

#	include "SceneNode3D.h"

class LightInterface;

class XmlElement;

namespace Menge
{
	// ? enum in loader ?
	const int LIGHT_POINT = 0;
	const int LIGHT_DIRECTIONAL = 1;
	const int LIGHT_SPOT = 2;

	class Light
		: public SceneNode3D
	{
	public:
		Light();
		~Light();
	public:
		void	setAttenuation( float _range, float _constant, float _linear, float _quadratic );
		float	getAttenuationRange() const;
		float	getAttenuationConstFactor() const;
		float	getAttenuationLinearFactor() const;
		float	getAttenuationQuadraticFactor() const;

		void	setSpotlightRange( float _innerAngle, float _outerAngle, float _falloff );
		float	getSpotlightInnerAngle() const;
		float	getSpotlightOuterAngle() const;
		float	getSpotlightFalloff() const;

		void	setDiffuseColour( float _r, float _g, float _b );
		void	setSpecularColour( float _r, float _g, float _b );

		bool	isVisible() const;
		void	setVisible( bool _enabled );
		void	setCastsShadows( bool _enabled );

		void	setDirection3D( float _x, float _y, float _z );

	public:
		void	loader( XmlElement * _xml );

	protected:
		bool	_activate() override;
		void	_deactivate() override;
		bool	_compile() override;
		void	_release() override;
		void	_update( float _timing ) override;
	
	private:
		LightInterface * m_lightInterface;
	
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