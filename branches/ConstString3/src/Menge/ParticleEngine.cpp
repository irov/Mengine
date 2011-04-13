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
								->openInputFile( _fileSystemName, Helper::to_str(_filename) );

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

			delete[] fileBuffer;

			return 0;
		}

		delete[] fileBuffer;

		return container;
	}
	////////////////////////////////////////////////////////////////////////////
	//EmitterInterface * ParticleEngine::createEmitterFromContainer( const ConstString & _name, const EmitterContainerInterface * _container )
	//{
	//	return m_interface->createEmitterFromContainer( Helper::to_str(_name), _container );
	//}
	////////////////////////////////////////////////////////////////////////////
	//void ParticleEngine::releaseEmitter( EmitterInterface * _emitter )
	//{
	//	return m_interface->releaseEmitter( _emitter );
	//}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEngine::flushEmitter( EmitterInterface * _emitter, TVectorParticleMeshes & _meshes, TVectorParticleVerices & _particles, int _particlesLimit )
	{
		return m_interface->flushParticles( _emitter, _meshes, _particles, _particlesLimit );
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
}
