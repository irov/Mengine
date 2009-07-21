#	include "ParticleEngine.h"
#	include "FileEngine.h"
#	include "LogEngine.h"
#	include "math/box2.h"
#	include "Viewport.h"

namespace Menge
{
	ParticleEngine::ParticleEngine( ParticleSystemInterface * _interface )
		: m_interface( _interface )
		, m_maxParticlesNum( 2000 )
		, m_frameParticlesNum( 0 )
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
	bool ParticleEngine::flushEmitter( EmitterInterface * _emitter, int _typeParticle, TVectorRenderParticle & _particles )
	{
		m_interface->lockEmitter( _emitter, _typeParticle );
		m_interface->flushParticles( _particles );
		m_interface->unlockEmitter( _emitter );

		return _particles.empty() == false;
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEngine::releaseEmitter( EmitterInterface * _emitter )
	{
		return m_interface->releaseEmitter( _emitter );
	}
	//////////////////////////////////////////////////////////////////////////
	String ParticleEngine::getTextureName( int _index ) const
	{
		return m_interface->getTextureName( _index );
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEngine::lockEmitter( EmitterInterface * _emitter, int _typeParticle )
	{
		return m_interface->lockEmitter( _emitter, _typeParticle );
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
	std::size_t ParticleEngine::getParticlesCount( const TVectorRenderParticle & _particles, EmitterInterface* _emitter, int _typeParticle, Viewport* _viewport, const mt::mat3f* _transform  )
	{
		std::size_t count = 0;
		//lockEmitter( _emitter, _typeParticle );

		for( TVectorRenderParticle::const_iterator
			it = _particles.begin(),
			it_end = _particles.end();
		it != it_end;
		++it )
		{
			const RenderParticle & p = *it;

			EmitterRectangle & rectangle = *(EmitterRectangle*)&p.rectangle;

			if( _transform != NULL )
			{
				mt::vec2f origin, transformX, transformY;
				mt::mul_v2_m3( origin, rectangle.v[0], *_transform );
				mt::mul_v2_m3_r( transformX, rectangle.v[1] - rectangle.v[0], *_transform );
				mt::mul_v2_m3_r( transformY, rectangle.v[3] - rectangle.v[0], *_transform );
				rectangle.v[0] = origin;
				rectangle.v[1] = rectangle.v[0] + transformX;
				rectangle.v[2] = rectangle.v[1] + transformY;
				rectangle.v[3] = rectangle.v[0] + transformY;
			}

			mt::box2f pbox;
			mt::reset( pbox, rectangle.v[0] );
			mt::add_internal_point( pbox, rectangle.v[1] );
			mt::add_internal_point( pbox, rectangle.v[2] );
			mt::add_internal_point( pbox, rectangle.v[3] );
			if( _viewport->testBBox( pbox ) == true )
			{
				++count;
			}
		}

		//unlockEmitter( _emitter );

		m_frameParticlesNum += count;
		size_t retCount = 0;

		if( m_frameParticlesNum > m_maxParticlesNum )
		{
			retCount = 0;
		}
		else
		{
			retCount = std::min( count, m_maxParticlesNum - m_frameParticlesNum );
		}
		return retCount;
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEngine::beginFrame()
	{
		m_frameParticlesNum = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ParticleEngine::getFrameParticlesCount() const
	{
		return m_frameParticlesNum;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ParticleEngine::getMaxParticlesCount() const
	{
		return m_maxParticlesNum;
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEngine::setMaxParticlesCount( size_t _count )
	{
		m_maxParticlesNum = _count;
	}
	//////////////////////////////////////////////////////////////////////////
	int ParticleEngine::getTextureCount() const
	{
		return m_interface->getTextureCount();
	}
	//////////////////////////////////////////////////////////////////////////
}
