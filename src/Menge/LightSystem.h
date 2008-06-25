#	pragma once

#	include "Math/vec2.h"
#	include <string>

#	include <list>

namespace Menge
{
	class Light2D;
	class ShadowCaster2D;

	class LightSystem
	{
	public:
		LightSystem();

	public:
		void update();
		void regLight( Light2D * _light );
		void regShadowCaster( ShadowCaster2D * _shadowCaster );

	private:

		typedef std::list<ShadowCaster2D*> TListShadowCaster;
		TListShadowCaster m_shadowCasters;

		typedef std::list<Light2D*> TListLight;
		TListLight m_lights;
	};
}