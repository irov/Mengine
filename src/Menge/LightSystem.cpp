#	include "LightSystem.h"

#	include "ShadowCaster2D.h"
#	include "Light2D.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	LightSystem::LightSystem()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void LightSystem::update()
	{
		for(TListLight::iterator it = m_lights.begin(); it != m_lights.end(); it++)
		{
			Light2D * light = *it;

			for(TListShadowCaster::iterator it = m_shadowCasters.begin(); it != m_shadowCasters.end(); it++)
			{
				light->makeShadow(*(*it));
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void LightSystem::regLight( Light2D * _light )
	{
		m_lights.push_back(_light);
	}
	//////////////////////////////////////////////////////////////////////////
	void LightSystem::regShadowCaster( ShadowCaster2D * _shadowCaster )
	{
		m_shadowCasters.push_back(_shadowCaster);
	}
	//////////////////////////////////////////////////////////////////////////
}