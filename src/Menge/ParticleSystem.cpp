#	include "ParticleSystem.h"
#	include "ObjectImplement.h"
#	include "ResourceManager.h"

#	include "RenderEngine.h"
#	include "Viewport.h"

#	include "PointEmitter.h"
#	include "BoxEmitter.h"
#	include "SimplePhysicAffector.h"
#	include "float.h"

#	include "Interface/RenderSystemInterface.h"

#	include "XmlParser/XmlParser.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(ParticleSystem);
	//////////////////////////////////////////////////////////////////////////
	ParticleSystem::ParticleSystem()
		: m_maxPoint(FLT_MIN,FLT_MIN)
		, m_minPoint(FLT_MAX,FLT_MAX)
	{
//		addEmitter(new BoxEmitter());
		//addEmitter(new PointEmitter());
		addAffector(new SimplePhysicAffector());
		resizePool(2000);
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
	///////////////////////////////////////////////////////////////////////////
	void ParticleSystem::resizePool(size_t N)
	{
		/*not correct */
		m_particlePool.resize(N);

		for( size_t i = 0; i < N; ++i )
        {
			m_freeParticleList.push_back( &m_particlePool[i] );
        }
	}
	///////////////////////////////////////////////////////////////////////////
	bool ParticleSystem::isVisible(const Viewport & _viewPort)
	{
		if (m_maxPoint.x < _viewPort.begin.x || m_minPoint.x > _viewPort.end.x ) return false;
		if (m_maxPoint.y < _viewPort.begin.y || m_minPoint.y > _viewPort.end.y ) return false;

		Holder<RenderEngine>::hostage()->drawLine2D(m_minPoint,m_maxPoint,0xffff00ff);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleSystem::_activate()
	{
		if( SceneNode2D::_activate() == false )
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

//		info = m_particleResource->getDesc();
		m_image = m_particleResource->getImage();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleSystem::_deactivate()
	{
		SceneNode2D::_deactivate();

		Holder<ResourceManager>::hostage()
			->releaseResource( m_particleResource );
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleSystem::loader(TiXmlElement *xml)
	{
		XML_FOR_EACH_TREE(xml)
		{
			XML_CHECK_NODE_FOR_EACH( "Emitters" )
			{
				XML_CHECK_NODE( "Emitter" )
				{
					int u =0;
				//	XML_VALUE_ATTRIBUTE( "Index", sq.index );
			
			//		addEmitter(new BoxEmitter());
				}
			}	
		}

		Renderable2D::loader(xml);
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleSystem::_debugRender()
	{};
	//////////////////////////////////////////////////////////////////////////
	Particle*	ParticleSystem::createParticle()
	{
		Particle * particle = m_freeParticleList.front();
		m_activeParticleList.splice(m_activeParticleList.end(), m_freeParticleList, m_freeParticleList.begin());
		return particle;
	}
	//////////////////////////////////////////////////////////////////////////
	void	ParticleSystem::update(float t)
	{
		std::list<Particle*>::iterator end = m_activeParticleList.end();

		for(std::list<Particle*>::iterator it = m_activeParticleList.begin(); it != end;)
		{
			Particle * particle = static_cast<Particle*>(*it);
			particle->m_timeToLive += t;

			if(particle->m_timeToLive >= particle->m_totalTime)
			{
				m_freeParticleList.splice(m_freeParticleList.end(), m_activeParticleList, it++);
			}
			else
			{
				++it;
			}
		}

		size_t emissionCount = 0;
		
		for(std::list<ParticleEmitter*>::iterator it = m_emitters.begin(); it != m_emitters.end(); ++it)
		{
			emissionCount+=(*it)->getEmissionCount(t);
		}

		if(emissionCount < m_freeParticleList.size())
		{
			for( size_t i = 0; i < emissionCount; ++i )
			{
				Particle * particle = createParticle();
				for(std::list<ParticleEmitter*>::iterator it = m_emitters.begin(); it != m_emitters.end(); ++it)
				{
					(*it)->emitt(t,particle);
				}
			}
		}

		for(std::list<ParticleAffector*>::iterator it = m_affectors.begin(); it != m_affectors.end(); ++it)
		{
			(*it)->affectParticles(t,m_activeParticleList);
		}
							
		for(std::list<Particle*>::iterator it = m_activeParticleList.begin(); it != m_activeParticleList.end(); ++it)
		{
			Particle * particle = *it;
			const mt::vec2f& pos = particle->m_position;

			if (pos.x > m_maxPoint.x) m_maxPoint.x = pos.x;
			if (pos.y > m_maxPoint.y) m_maxPoint.y = pos.y;
				
			if (pos.x < m_minPoint.x) m_minPoint.x = pos.x;
			if (pos.y < m_minPoint.y) m_minPoint.y = pos.y;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleSystem::_render( const mt::mat3f &rwm, const Viewport & _viewPort ) 
	{
		if(m_activeParticleList.empty())
		{
			return;
		}

		std::list<Particle*>::iterator it = m_activeParticleList.begin();

		for( size_t i = 0; i < m_activeParticleList.size(); ++i )
		{
			Particle * particle = *it;

			mt::mat3f _transform;

			mt::ident_m3(_transform);

			float v = particle->m_rotation*particle->m_timeToLive;
		
			_transform.v2.x = particle->m_position.x;
			_transform.v2.y = particle->m_position.y;

			mt::vec2f _size((float)m_image->getWidth(),(float)m_image->getHeight());

            _size *= particle->m_size;

			unsigned int col = 
				(unsigned int(particle->m_color.a*255.0f)<<24) + 
				(unsigned int(particle->m_color.r*255.0f)<<16) + 
				(unsigned int(particle->m_color.g*255.0f)<<8) + 
				unsigned int(particle->m_color.b*255.0f);

			Holder<RenderEngine>::hostage()->renderImage(_transform,mt::vec2f(0,0),mt::vec4f(0,0,1.0f,1.0f),_size,col,m_image);
			it++;
		}
	}
}