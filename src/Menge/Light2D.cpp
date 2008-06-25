#	include "Light2D.h"

#	include "ObjectImplement.h"

#	include "XmlEngine.h"

#	include "LightSystem.h"

#	include "RenderEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(Light2D);
	//////////////////////////////////////////////////////////////////////////
	Light2D::Light2D()
		: m_chain(0,0)
		, m_shadowLength(200)
		, m_shadowPenumbraR(20)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Light2D::isVisible( const Viewport & _viewPort )
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Light2D::loader( XmlElement * _xml )
	{
		SceneNode2D::loader(_xml);

		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE( "PenumbraRadius", "Value", m_shadowPenumbraR );
			XML_CASE_ATTRIBUTE_NODE( "ShadowLength", "Value", m_shadowLength );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Light2D::_render( bool _enableDebug )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Light2D::_activate()
	{
		if( SceneNode2D::_activate() == false )
		{
			return false;
		}

		Holder<LightSystem>::hostage()->regLight(this);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Light2D::_deactivate()
	{
		SceneNode2D::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Light2D::_compile()
	{
		if( SceneNode2D::_compile() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Light2D::_release()
	{
		SceneNode2D::_release();
	}
	//////////////////////////////////////////////////////////////////////////
	void Light2D::_update( float _timing )
	{
		SceneNode2D::_update(_timing);
	}
	//////////////////////////////////////////////////////////////////////////
	void Light2D::setShadowLength(float _length)
	{
		m_shadowLength = _length;
	}
	//////////////////////////////////////////////////////////////////////////
	void Light2D::setPenumbraRadius(float _radius)
	{
		m_shadowPenumbraR = _radius;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Light2D::isVisibleEdge(const ShadowCaster2D & _caster, int i) const
	{
		mt::vec2f edge = _caster.getEdge(i);
		mt::vec2f normal = mt::perp(edge);

		mt::vec2f vertex = _caster.getVertex(i);
		mt::vec2f light_dir = vertex - getLocalPosition();
		float dot = mt::dot_v2_v2(light_dir,normal);	

		return dot < 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void Light2D::drawUmbra(const ShadowCaster2D & _caster, int first, int last)
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
			q.v[1] = _caster.getVertex(i) + (_caster.getVertex(i) - getLocalPosition()) * m_shadowLength;
			q.v[2] = _caster.getVertex(i+1) + (_caster.getVertex(next) - getLocalPosition()) * m_shadowLength;
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
	void Light2D::drawPenumbra(const ShadowCaster2D & _caster, int first, int last)
	{
		mt::vec2f v[3];

		mt::vec2f light = _caster.getVertex(first) - getLocalPosition();

		v[0] = _caster.getVertex(first);
		v[1] = _caster.getVertex(first) + light * m_shadowLength;
		v[2] = v[1] + m_shadowPenumbraR * mt::perp(light);

		RenderEngine* reng = Holder<RenderEngine>::hostage();

		reng->renderLine(0xFFFFFFFF, v[0], v[1] );
		reng->renderLine(0xFFFFFFFF, v[1], v[2] );
		reng->renderLine(0xFFFFFFFF, v[2], v[0] );

		light = _caster.getVertex(last) - getLocalPosition();

		v[0] = _caster.getVertex(last);
		v[1] = _caster.getVertex(last) + light * m_shadowLength;
		v[2] = v[1] - m_shadowPenumbraR * mt::perp(light);

		reng->renderLine(0xFFFFFFFF, v[0], v[1] );
		reng->renderLine(0xFFFFFFFF, v[1], v[2] );
		reng->renderLine(0xFFFFFFFF, v[2], v[0] );
	}
	//////////////////////////////////////////////////////////////////////////
	void Light2D::makeShadow(const ShadowCaster2D & _caster)
	{
		int shadow_chain_first = -1;
		int shadow_chain_last = -1;

		int size = _caster.size();

		for(int i = 0; i < m_chain.first + size; i++)
		{
			int next = (i + 1) % size;

			bool visible = isVisibleEdge(_caster,i % size);
			bool not_visible = isVisibleEdge( _caster, next ) == false;

			if(visible && not_visible)
			{
				shadow_chain_first = next;
				m_chain.first = next;
				break;
			}
		}

		for(int i = 0; i < m_chain.second + size; i++)
		{
			int next = (i + 1) % size;

			bool visible = isVisibleEdge(_caster,i % size) == false;
			bool not_visible = isVisibleEdge( _caster, next );

			if(visible && not_visible)
			{
				shadow_chain_last = next;
				m_chain.second = next;
				break;
			}
		}

		if((shadow_chain_first != -1) && (shadow_chain_last != -1))
		{
			int size = _caster.size();

			if(shadow_chain_first > shadow_chain_last) 
			{
				shadow_chain_last += size;
				std::swap(m_chain.first,m_chain.second);
			}

			drawUmbra(_caster, shadow_chain_first, shadow_chain_last);
			drawPenumbra(_caster, shadow_chain_first, shadow_chain_last);
		}
	}
}