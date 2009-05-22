#	include "ParticleEngine.h"
#	include "FileEngine.h"
#	include "LogEngine.h"
#	include "math/box2.h"
#	include "Viewport.h"

namespace Menge
{
	ParticleEngine::ParticleEngine( ParticleSystemInterface * _interface )
	: m_interface( _interface )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ParticleEngine::~ParticleEngine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	EmitterContainerInterface * ParticleEngine::createEmitterContainerFromFile( const String & _filename )
	{
		FileBuffer fb = Holder<FileEngine>::hostage()->getFileBuffer( _filename );

		if( fb.buffer == 0 )
		{
			MENGE_LOG_ERROR( "ParticleEngine can't open file \"%s\""
				, _filename.c_str() );
			return 0;
		}

		EmitterContainerInterface * container = m_interface->createEmitterContainerFromMemory( fb.buffer );

		if( container == 0 )
		{
			MENGE_LOG_ERROR( "ParticleEngine can't create emitter container \"%s\""
				, _filename.c_str() );
			return 0;
		}

		return container;
	}
	//////////////////////////////////////////////////////////////////////////
	EmitterInterface * ParticleEngine::createEmitterFromContainer( const String & _name, const EmitterContainerInterface * _container )
	{
		return m_interface->createEmitterFromContainer( _name, _container );
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEngine::lockEmitter( EmitterInterface * _emitter, int _typeParticle )
	{
		return m_interface->lockEmitter( _emitter, _typeParticle );
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEngine::releaseEmitter( EmitterInterface * _emitter )
	{
		return m_interface->releaseEmitter( _emitter );
	}
	//////////////////////////////////////////////////////////////////////////
	String ParticleEngine::getTextureName() const
	{
		return m_interface->getTextureName();
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEngine::nextParticle( RenderParticle & _particle )
	{
		return m_interface->nextParticle( _particle );
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEngine::unlockEmitter( EmitterInterface * _emitter )
	{
		return m_interface->unlockEmitter( _emitter );
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEngine::releaseEmitterContainer( EmitterContainerInterface* _containerInterface )
	{
		m_interface->releaseEmitterContainer( _containerInterface );
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ParticleEngine::getParticlesCount( EmitterInterface* _emitter, int _typeParticle, Viewport* _viewport, const mt::mat3f* _transform  )
	{
		std::size_t count = 0;
		lockEmitter( _emitter, _typeParticle );
		RenderParticle p;
		
		mt::box2f pbox;

		while( nextParticle( p ) )
		{
			mt::vec2f vertices[4] =
			{
				mt::vec2f(p.x2, p.y2),
				mt::vec2f(p.x1, p.y1),
				mt::vec2f(p.x4, p.y4),
				mt::vec2f(p.x3, p.y3)
			};

			if( _transform != NULL )
			{
				mt::vec2f origin, transformX, transformY;
				mt::mul_v2_m3( origin, vertices[0], *_transform );
				mt::mul_v2_m3_r( transformX, vertices[1] - vertices[0], *_transform );
				mt::mul_v2_m3_r( transformY, vertices[3] - vertices[0], *_transform );
				vertices[0] = origin;
				vertices[1] = vertices[0] + transformX;
				vertices[2] = vertices[1] + transformY;
				vertices[3] = vertices[0] + transformY;
			}

			mt::reset( pbox, vertices[0] );
			mt::add_internal_point( pbox, vertices[1] );
			mt::add_internal_point( pbox, vertices[2] );
			mt::add_internal_point( pbox, vertices[3] );
			if( _viewport->testBBox( pbox ) == true )
			{
				++count;
			}
		}
		unlockEmitter( _emitter );
		return count;
	}
	//////////////////////////////////////////////////////////////////////////
}
