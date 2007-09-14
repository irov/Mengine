#	include "ParticleSystem.h"
#	include "ObjectImplement.h"
#	include "ResourceManager.h"

#	include "RenderEngine.h"
#	include "Viewport.h"

#	include "PointEmitter.h"
#	include "BoxEmitter.h"
#	include "SimplePhysicAffector.h"

#	include "Interface/RenderSystemInterface.h"

#	include "XmlParser/XmlParser.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(ParticleSystem);
	//////////////////////////////////////////////////////////////////////////
	ParticleSystem::ParticleSystem()
	{
//		addEmitter(new BoxEmitter());
		//addEmitter(new PointEmitter());
		addAffector( new SimplePhysicAffector() );
		_resizePool(2000);
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
		SceneNode2D::loader(xml);

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
	}
	//////////////////////////////////////////////////////////////////////////
	Particle *	ParticleSystem::_createParticle()
	{
		Particle * particle = m_freeParticleList.front();
		m_activeParticleList.splice( m_activeParticleList.end(), m_freeParticleList, m_freeParticleList.begin() );
		return particle;
	}
	//////////////////////////////////////////////////////////////////////////
	void	ParticleSystem::_doLife( float _timing )
	{
		TListActiveParticle::iterator end = m_activeParticleList.end();

		for( TListActiveParticle::iterator it = m_activeParticleList.begin(); it != end; )
		{
			Particle * particle = static_cast<Particle*>(*it);

			particle->m_timeToLive += _timing;

			if( particle->m_timeToLive >= particle->m_totalTime )
			{
				m_freeParticleList.splice( m_freeParticleList.end(), m_activeParticleList, it++ );
			}
			else
			{
				++it;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	size_t	ParticleSystem::_getEmissionCount( float _timing )
	{
		size_t emissionCount = 0;

		for each( ParticleEmitter * emitter in m_emitters )
		{
			emissionCount += emitter->getEmissionCount( _timing );
		}

		return emissionCount;
	}
	//////////////////////////////////////////////////////////////////////////
	void	ParticleSystem::_processEmitters( float _timing )
	{
		size_t emissionCount = _getEmissionCount( _timing );

		if( emissionCount < m_freeParticleList.size() )
		{
			for( size_t i = 0; i < emissionCount; ++i )
			{
				Particle * particle = _createParticle();

				for each( ParticleEmitter * emitter in m_emitters )
				{
					emitter->emitt( _timing, particle );
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void	ParticleSystem::_processAffectors( float _timing )
	{
		for each( ParticleAffector * affector in m_affectors )
		{
			affector->affectParticles( _timing, m_activeParticleList );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void	ParticleSystem::_calculateBoundingBox()
	{
		for each( Particle * particle in m_activeParticleList )
		{
			mt::add_internal_point( m_bbox, particle->m_position );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void	ParticleSystem::update( float _timing )
	{
		_doLife( _timing );
		_processEmitters( _timing );
		_processAffectors( _timing );
		_calculateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleSystem::_render( const mt::mat3f &rwm, const Viewport & _viewPort ) 
	{
		if( m_activeParticleList.empty() )
		{
			return;
		}

		TListActiveParticle::iterator it = m_activeParticleList.begin();

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
	//////////////////////////////////////////////////////////////////////////
	void ParticleSystem::_debugRender()
	{};
	//////////////////////////////////////////////////////////////////////////
	void	ParticleSystem::addAffector( ParticleAffector * _aff )
	{
		m_affectors.push_back(_aff);
	}
	//////////////////////////////////////////////////////////////////////////
	void	ParticleSystem::addEmitter( ParticleEmitter * _em )
	{
		m_emitters.push_back(_em);
	}
	///////////////////////////////////////////////////////////////////////////
	void ParticleSystem::_resizePool(size_t N)
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
		if (m_bbox.MaxEdge.x < _viewPort.begin.x || m_bbox.MinEdge.x > _viewPort.end.x ) return false;
		if (m_bbox.MaxEdge.y < _viewPort.begin.y || m_bbox.MinEdge.y > _viewPort.end.y ) return false;

		Holder<RenderEngine>::hostage()->drawLine2D( m_bbox.MinEdge, m_bbox.MaxEdge, 0xffff00ff );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}