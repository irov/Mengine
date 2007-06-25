#	include "ParticleSystem.h"
#	include "ObjectImplement.h"
#	include "XmlParser.h"
#	include "ResourceManager.h"
#	include "RenderSystemInterface.h"
#	include "Holder.h"
#	include "RenderEngine.h"

#	include "PointEmitter.h"
#	include "SimplePhysicAffector.h"

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
	void	ParticleSystem::addAffector(ParticleAffector* _aff)
	{
		m_affectors.push_back(_aff);
	}
	//////////////////////////////////////////////////////////////////////////
	void	ParticleSystem::addEmitter(ParticleEmitter* _em)
	{
		m_emitters.push_back(_em);
	}
	//////////////////////////////////////////////////////////////////////////
	ParticleSystem::ParticleSystem()
		: alive_num(0)
	{
		addEmitter(new PointEmitter());
		addAffector(new SimplePhysicAffector());

		m_particlePool.reserve(200);
		m_particlePool.resize(200);
		
		for( size_t i = 0; i < m_particlePool.size(); ++i )
		{
            m_particlePool[i] = new Particle();
		}

		for( size_t i = 0; i < m_particlePool.size(); ++i )
        {
			m_freeParticleList.push_back( m_particlePool[i] );
        }
	}
	///////////////////////////////////////////////////////////////////////////
	bool ParticleSystem::isVisible(const Viewport & _viewPort)
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleSystem::_activate()
	{
		if( Renderable::_activate() == false )
		{
			return false;
		}

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
		Renderable::_deactivate();
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
		std::list<Particle*>::iterator it;
		std::list<Particle*>::iterator end = m_activeParticleList.end();

/*		for(it = m_activeParticleList.begin(); it != end;)
		{
			Particle * particle = static_cast<Particle*>(*it);
			particle->m_timeToLive += t;

			if(particle->m_timeToLive >= particle->m_totalTime)
			{
			//	m_freeParticleList.splice(m_freeParticleList.end(), m_activeParticleList, it++);
				++it;
			}
			else
			{
				++it;
			}
		}
*/
		int emissionCount = 0;
		
		for(std::list<ParticleEmitter*>::iterator it = m_emitters.begin(); it != m_emitters.end(); ++it)
		{
			emissionCount+=(*it)->getEmissionCount(t);
		}

		//if(emissionCount <= m_freeParticleList.size())
		//{
			for(int i = 0; i < emissionCount; i++)
			{
				Particle * particle = createParticle();
				for(std::list<ParticleEmitter*>::iterator it = m_emitters.begin(); it != m_emitters.end(); ++it)
				{
					(*it)->emitt(t,particle);
				}
			}
		//}

		if(m_activeParticleList.empty())
		{
			return;
		}

		for(std::list<ParticleAffector*>::iterator it = m_affectors.begin(); it != m_affectors.end(); ++it)
		{
			(*it)->affectParticles(t,m_activeParticleList);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleSystem::_render( const mt::mat3f &rwm, const Viewport & _viewPort ) 
	{
		if(m_activeParticleList.empty())
		{
			return;
		}

		Particle* particle = *m_activeParticleList.begin();

		for(int i = 0; i < m_activeParticleList.size(); i++)
		{
			mt::mat3f _transform;

			mt::ident_m3(_transform);

			float v = particle->m_rotation*particle->m_timeToLive;
		
			_transform.v2.x = particle->m_position.x;
			_transform.v2.y = particle->m_position.y;

			mt::vec2f _size(m_image->getWidth(),m_image->getHeight());

            _size *= particle->m_size;

			unsigned int col = (DWORD(particle->m_color.a*255.0f)<<24) + (DWORD(particle->m_color.r*255.0f)<<16) + (DWORD(particle->m_color.g*255.0f)<<8) + DWORD(particle->m_color.b*255.0f);

			Holder<RenderEngine>::hostage()->renderImage(_transform,mt::vec2f(0,0),mt::vec4f(0,0,1.0f,1.0f),_size,col,m_image);
			particle++;
		}
	}
}