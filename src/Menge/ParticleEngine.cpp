#	include "ParticleEngine.h"
#	include "FileEngine.h"
#	include "LogEngine.h"

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
	void ParticleEngine::releaseEmitterContainer( EmitterContainerInterface* _containerInterface )
	{
		m_interface->releaseEmitterContainer( _containerInterface );
	}
	//////////////////////////////////////////////////////////////////////////
}
