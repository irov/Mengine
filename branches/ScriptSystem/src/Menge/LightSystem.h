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

		struct ShadowCasterCache
		{
			std::pair<std::size_t, std::size_t> m_chain;
			ShadowCaster2D * m_shadowCaster;
		};

		typedef std::list<ShadowCasterCache> TListShadowCaster;

		//typedef std::list<ShadowCaster2D*> TListShadowCaster;
		TListShadowCaster m_shadowCasters;

		typedef std::list<Light2D*> TListLight;
		TListLight m_lights;

		bool isVisibleEdge(const Light2D & _light, const ShadowCaster2D & _caster, std::size_t i) const;
		void drawUmbra(const Light2D & _light, const ShadowCaster2D & _caster, std::size_t first, std::size_t last);
		void drawPenumbra(const Light2D & _light, const ShadowCaster2D & _caster, std::size_t first, std::size_t last);
		void makeShadow(const Light2D & _light, ShadowCasterCache & _cell);
	};
}
