#	include "ParticleSystem.h"
#	include "ObjectImplement.h"
#	include "XmlParser.h"
#	include "ResourceManager.h"
#	include "RenderSystemInterface.h"
#	include "Holder.h"
#	include "RenderEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(ParticleSystem);
	//////////////////////////////////////////////////////////////////////////
	float rand2(float min, float max)
	{
		float r = (float)rand() / RAND_MAX;
		return min + r * (max-min);
	}
	//////////////////////////////////////////////////////////////////////////
	ParticleSystem::ParticleSystem()
		: emission_residue(0.0f)
		, alive_num(0)
		, fAge(-1)
		, position(0,0)
		, prev_position(0,0)
	{
	}
	///////////////////////////////////////////////////////////////////////////
	bool ParticleSystem::isVisible(const Viewport & _viewPort)
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleSystem::_activate()
	{
		m_particleResource = 
		Holder<ResourceManager>::hostage()
			->getResourceT<ResourceParticle>( m_resourceName );

		if( m_particleResource == 0 )
		{
			return false;
		}

		info = m_particleResource->getDesc();
		m_image = m_particleResource->getImage();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleSystem::_deactivate()
	{
		Holder<ResourceManager>::hostage()
			->releaseResource( m_particleResource );
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleSystem::loader(TiXmlElement *xml)
	{
		XML_FOR_EACH_TREE(xml)
		{
			XML_CHECK_VALUE_NODE( "ResourceName", "Value", m_resourceName );
		}

		Renderable::loader(xml);
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleSystem::_debugRender()
	{};
	//////////////////////////////////////////////////////////////////////////
	void	ParticleSystem::update(float t)
	{
		move(300,300);
		if(fAge >= 0)
		{
			fAge += t;
			if(fAge >= info.lifetime) fAge = -2.0f;
		}

		Particle * particle = particles;

		for(int i = 0; i < alive_num; i++)
		{
			particle->age += t;

			if(particle->age >= particle->terminal_age)
			{
				alive_num--;
				memcpy(particle, &particles[alive_num], sizeof(Particle));
				i--;
				continue;
			}

			particle->integrate(position,t);
			particle++;
		}

		if(fAge != -2.0f)
		{
			float	particles_needed = info.emission * t + emission_residue;
			int		particles_created = (int)particles_needed;
			emission_residue = particles_needed - particles_created;

			Particle * particle = &particles[alive_num];

			for(int i = 0; i < particles_created; i++)
			{
				if(alive_num >= MAX_PARTICLES) break;

				particle->age = 0.0f;
				particle->terminal_age = rand2(info.life.x, info.life.y);

				particle->x = prev_position+(position-prev_position)*rand2(0.0f, 1.0f);

				float ang = info.dir-1.57f+rand2(0,info.spread)-info.spread/2.0f;

				particle->v.x = cosf(ang);
				particle->v.y = sinf(ang);
				particle->v *= rand2(info.v.x, info.v.y);

				particle->g = rand2(info.g.x, info.g.y);
				particle->r = rand2(info.r.x, info.r.y);
				particle->tan = rand2(info.tan.x, info.tan.y);

				particle->size = rand2(info.size.x, info.size.x+(info.size.y-info.size.x)*info.size.z);
				particle->size_dt = (info.size.y-particle->size) / particle->terminal_age;

				particle->spin = rand2(info.spin.x, info.spin.x+(info.spin.y-info.spin.x)*info.spin.z);
				particle->spin_dt = (info.spin.y-particle->spin) / particle->terminal_age;

				particle->color.r = rand2(info.startColor.r, info.startColor.r+(info.endColor.r-info.startColor.r)*info.varColor.x);
				particle->color.g = rand2(info.startColor.g, info.startColor.g+(info.endColor.g-info.startColor.g)*info.varColor.x);
				particle->color.b = rand2(info.startColor.b, info.startColor.b+(info.endColor.b-info.startColor.b)*info.varColor.x);
				particle->color.a = rand2(info.startColor.a, info.startColor.a+(info.endColor.a-info.startColor.a)*info.varColor.y);

				particle->color_dt = (info.endColor-particle->color) / particle->terminal_age;

				alive_num++;
				particle++;
			}
		}

		prev_position = position;
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleSystem::_render( const mt::mat3f &rwm, const Viewport & _viewPort ) 
	{
		Particle * particle = particles;

		for(int i = 0; i < alive_num; i++)
		{

			mt::mat3f _transform;

			mt::ident_m3(_transform);

			float v = particle->spin*particle->age;

		/*	_transform.v0.x = cosf(v);
			_transform.v0.y = -sinf(v);

			_transform.v1.x = sinf(v);
			_transform.v1.y = cosf(v);
*/
			_transform.v2.x = particle->x.x;
			_transform.v2.y = particle->x.y;

		//	_transform = _transform * rwm;
		
			mt::vec2f _size(m_image->getWidth(),m_image->getHeight());

			unsigned int col = (DWORD(particle->color.a*255.0f)<<24) + (DWORD(particle->color.r*255.0f)<<16) + (DWORD(particle->color.g*255.0f)<<8) + DWORD(particle->color.b*255.0f);

			Holder<RenderEngine>::hostage()->renderImage(_transform,mt::vec2f(0,0),mt::vec4f(0,0,1.0f,1.0f),_size,col,m_image);
			particle++;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleSystem::move(float x, float y)
	{
		prev_position = position;
		position = mt::vec2f(x,y);
	}
}