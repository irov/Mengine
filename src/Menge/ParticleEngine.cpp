#	include "ParticleEngine.h"
#	include "FileEngine.h"
#	include "LogEngine.h"
#	include "math/box2.h"
#	include "Viewport.h"

namespace Menge
{
	ParticleEngine::ParticleEngine()
		: m_interface( NULL )
		, m_maxParticlesNum( 2000 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ParticleEngine::~ParticleEngine()
	{
		if( m_interface != NULL )
		{
			releaseInterfaceSystem( m_interface );
			m_interface = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEngine::initialize()
	{
		bool result = initInterfaceSystem( &m_interface );
		if( ( result == false ) || ( m_interface == NULL ) )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	EmitterContainerInterface * ParticleEngine::createEmitterContainerFromFile( const String& _fileSystemName, const String & _filename )
	{
		FileInput* file = FileEngine::hostage()
								->openFileInput( _fileSystemName, _filename );

		if( file == NULL )
		{
			MENGE_LOG_ERROR( "ParticleEngine can't open file '%s'"
				, _filename.c_str() 
				);

			return 0;
		}

		int fileSize = file->size();
		unsigned char* fileBuffer = new unsigned char[fileSize];
		file->read( fileBuffer, fileSize );

		FileEngine::hostage()
			->closeFileInput( file );

		EmitterContainerInterface * container = m_interface->createEmitterContainerFromMemory( fileBuffer );

		if( container == NULL )
		{
			MENGE_LOG_ERROR( "ParticleEngine can't create emitter container '%s'"
				, _filename.c_str() 
				);
		}

		delete[] fileBuffer;

		return container;
	}
	//////////////////////////////////////////////////////////////////////////
	EmitterInterface * ParticleEngine::createEmitterFromContainer( const String & _name, const EmitterContainerInterface * _container )
	{
		return m_interface->createEmitterFromContainer( _name, _container );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEngine::flushEmitter( EmitterInterface * _emitter, int _typeParticle, TVectorRenderParticle & _particles, int* _texturesNum )
	{
		if( m_interface->lockEmitter( _emitter, _typeParticle ) == false )
		{
			const std::string & emitterName = _emitter->getName();

			MENGE_LOG_ERROR( "ParticleEngine can't flush emitter '%s' type '%d'"
				, emitterName.c_str()
				, _typeParticle
				);

			return false;
		}

		if( _texturesNum !=	NULL )
		{
			*_texturesNum = m_interface->getTextureCount();
		}

		if( *_texturesNum == 0 )
		{
			m_interface->unlockEmitter( _emitter );
			return false;
		}

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
	bool ParticleEngine::lockEmitter( EmitterInterface * _emitter, int _typeParticle )
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
	void ParticleEngine::beginFrame()
	{
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
