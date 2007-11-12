#	include "ParticleEngine.h"

namespace Menge
{
	ParticleEngine::ParticleEngine( ParticleSystemInterface * _interface )
	: m_interface( _interface )
	{
		Holder<ParticleEngine>::keep( this );
	}
	//////////////////////////////////////////////////////////////////////////
	ParticleEngine::~ParticleEngine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	EmitterContainerInterface * ParticleEngine::createEmitterContainerFromMemory( void * _buffer )
	{
		return m_interface->createEmitterContainerFromMemory( _buffer );
	}
	//////////////////////////////////////////////////////////////////////////
	EmitterInterface * ParticleEngine::createEmitterFromContainer( const char * _name, const EmitterContainerInterface * _container )
	{
		return m_interface->createEmitterFromContainer( _name, _container );
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEngine::lockEmitter( EmitterInterface * _emitter, int _typeParticle )
	{
		return m_interface->lockEmitter( _emitter, _typeParticle );
	}
	//////////////////////////////////////////////////////////////////////////
	RenderParticle * ParticleEngine::nextParticle()
	{
		return m_interface->nextParticle();
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEngine::unlockEmitter( EmitterInterface * _emitter )
	{
		return m_interface->unlockEmitter( _emitter );
	}
	//////////////////////////////////////////////////////////////////////////
}