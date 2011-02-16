#	include "ParticleEngine.h"
#	include "FileEngine.h"
#	include "Logger/Logger.h"
#	include "math/box2.h"
#	include "Core/Viewport.h"

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
	EmitterContainerInterface * ParticleEngine::createEmitterContainerFromFile( const ConstString& _fileSystemName, const ConstString & _filename )
	{
		FileInputInterface* file = FileEngine::get()
								->openInputFile( _fileSystemName, to_str(_filename) );

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

		file->close();

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
	EmitterInterface * ParticleEngine::createEmitterFromContainer( const ConstString & _name, const EmitterContainerInterface * _container )
	{
		return m_interface->createEmitterFromContainer( to_str(_name), _container );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEngine::flushEmitter( EmitterInterface * _emitter, int _typeParticle, TVectorRenderParticle & _particles, int & _texturesNum, int & _particlesNum, int _particlesLimit )
	{
		_texturesNum = this->lockEmitter( _emitter, _typeParticle );

		if( _texturesNum ==	0 )
		{
			return false;
		}

		_particlesNum = m_interface->flushParticles( _particles, _particlesLimit );
		
		this->unlockEmitter( _emitter );

		return _particlesNum != 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEngine::releaseEmitter( EmitterInterface * _emitter )
	{
		return m_interface->releaseEmitter( _emitter );
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEngine::getEmitterPosition( EmitterInterface * _emitter, mt::vec2f & _pos )
	{
		m_interface->getEmitterPosition( _emitter, _pos );
	}
	//////////////////////////////////////////////////////////////////////////
	const char * ParticleEngine::getTextureName( int _index ) const
	{
		return m_interface->getTextureName( _index );
	}
	//////////////////////////////////////////////////////////////////////////
	int ParticleEngine::lockEmitter( EmitterInterface * _emitter, int _typeParticle )
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

		int textureCount = m_interface->getTextureCount();

		if( textureCount == 0 )
		{
			this->unlockEmitter( _emitter );
			return false;
		}

		return textureCount;
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
	void ParticleEngine::beginParticlesCount()
	{
		m_renderParticleNum = m_maxParticlesNum;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ParticleEngine::renderParticlesCount( size_t _count )
	{
		return m_renderParticleNum - _count;
	}
	//////////////////////////////////////////////////////////////////////////
	int ParticleEngine::getTextureCount() const
	{
		return m_interface->getTextureCount();
	}
	//////////////////////////////////////////////////////////////////////////
}
