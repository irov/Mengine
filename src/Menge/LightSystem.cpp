#	include "LightSystem.h"

#	include "ShadowCaster2D.h"
#	include "Light2D.h"
#	include "RenderEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	LightSystem::LightSystem()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void LightSystem::update()
	{
		// ÇÀÞÇÀÒÜ HGE!
		for(TListLight::iterator it = m_lights.begin(); it != m_lights.end(); it++)
		{
			Light2D * light = *it;

			//static int i = 0;
			//light->setLocalPosition(mt::vec2f(i++,i++));
			for(TListShadowCaster::iterator it = m_shadowCasters.begin(); it != m_shadowCasters.end(); it++)
			{
				makeShadow(*light,*it);
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
		ShadowCasterCache cell = {std::make_pair(0,0), _shadowCaster};
		m_shadowCasters.push_back(cell);
	}
	//////////////////////////////////////////////////////////////////////////
	bool LightSystem::isVisibleEdge(const Light2D & _light, const ShadowCaster2D & _caster, int i) const
	{
		mt::vec2f edge = _caster.getEdge(i);
		mt::vec2f normal = mt::perp(edge);

		mt::vec2f vertex = _caster.getVertex(i);
		mt::vec2f light_dir = vertex - _light.getLocalPosition();
		float dot = mt::dot_v2_v2(light_dir,normal);	

		return dot < 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void LightSystem::drawUmbra(const Light2D & _light, const ShadowCaster2D & _caster, int first, int last)
	{
		struct quad
		{
			mt::vec2f v[4];
		};

		std::vector<quad> _shadow;

		int size = _caster.size();

		for(int i = first; i < last; i++)				
		{
			int next = (i + 1) % size;

			quad q;

			q.v[0] = _caster.getVertex(i);
			q.v[1] = _caster.getVertex(i) + (_caster.getVertex(i) - _light.getLocalPosition()) * _light.getShadowLength();
			q.v[2] = _caster.getVertex(i+1) + (_caster.getVertex(next) - _light.getLocalPosition()) * _light.getShadowLength();
			q.v[3] = _caster.getVertex(i+1);

			_shadow.push_back(q);
		}

		RenderEngine* reng = Holder<RenderEngine>::hostage();

		for(int i = 0; i < _shadow.size(); i++)
		{
			quad q = _shadow[i];
			reng->renderLine(0xFFFFFFFF, q.v[0], q.v[1] );

			q = _shadow[i];
			reng->renderLine(0xFFFFFFFF, q.v[1], q.v[2] );

			q = _shadow[i];
			reng->renderLine(0xFFFFFFFF, q.v[2], q.v[3] );

			q = _shadow[i];
			reng->renderLine(0xFFFFFFFF, q.v[3], q.v[0] );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void LightSystem::drawPenumbra(const Light2D & _light, const ShadowCaster2D & _caster, int first, int last)
	{
		mt::vec2f v[3];

		mt::vec2f light = _caster.getVertex(first) - _light.getLocalPosition();

		v[0] = _caster.getVertex(first);
		v[1] = _caster.getVertex(first) + light * _light.getShadowLength();
		v[2] = v[1] + _light.getPenumbraRadius() * mt::perp(light);

		RenderEngine* reng = Holder<RenderEngine>::hostage();

		reng->renderLine(0xFFFFFFFF, v[0], v[1] );
		reng->renderLine(0xFFFFFFFF, v[1], v[2] );
		reng->renderLine(0xFFFFFFFF, v[2], v[0] );

		light = _caster.getVertex(last) - _light.getLocalPosition();

		v[0] = _caster.getVertex(last);
		v[1] = _caster.getVertex(last) + light * _light.getShadowLength();
		v[2] = v[1] - _light.getPenumbraRadius() * mt::perp(light);

		reng->renderLine(0xFFFFFFFF, v[0], v[1] );
		reng->renderLine(0xFFFFFFFF, v[1], v[2] );
		reng->renderLine(0xFFFFFFFF, v[2], v[0] );
	}
	//////////////////////////////////////////////////////////////////////////
	void LightSystem::makeShadow(const Light2D & _light, ShadowCasterCache & _cell)
	{
		int shadow_chain_first = -1;
		int shadow_chain_last = -1;

		ShadowCaster2D & _caster = *_cell.m_shadowCaster;

		int size = _caster.size();

		std::pair<int,int> & chain = _cell.m_chain;

		for(int i = chain.first; i < chain.first + size; i++)
		{
			int next = (i + 1) % size;

			bool visible = isVisibleEdge(_light, _caster,i % size);
			bool not_visible = isVisibleEdge( _light, _caster, next ) == false;

			if(visible && not_visible)
			{
				shadow_chain_first = next;
				chain.first = next;
				break;
			}
		}

		for(int i = chain.second; i < chain.second + size; i++)
		{
			int next = (i + 1) % size;

			bool visible = isVisibleEdge(_light, _caster,i % size) == false;
			bool not_visible = isVisibleEdge(_light, _caster, next );

			if(visible && not_visible)
			{
				shadow_chain_last = next;
				chain.second = next;
				break;
			}
		}

		if((shadow_chain_first != -1) && (shadow_chain_last != -1))
		{
			int size = _caster.size();

			if(shadow_chain_first > shadow_chain_last) 
			{
				shadow_chain_last += size;
			}

			drawUmbra(_light, _caster, shadow_chain_first, shadow_chain_last);
			drawPenumbra(_light, _caster, shadow_chain_first, shadow_chain_last);
		}
	}
}