#	include "ParticleEngine.h"

#   include "Interface/FileSystemInterface.h"

#	include "Logger/Logger.h"

#	include "Core/Viewport.h"

#	include "math/box2.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ParticleService, Menge::ParticleServiceInterface, Menge::ParticleEngine );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	ParticleEngine::ParticleEngine()
		: m_serviceProvider(NULL)
        , m_particleSystem(NULL)
		, m_maxParticlesNum(2000)
		, m_renderParticleNum(0)        
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ParticleEngine::~ParticleEngine()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void ParticleEngine::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;

        m_particleSystem = PARTICLE_SYSTEM( m_serviceProvider );
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * ParticleEngine::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	EmitterContainerInterface * ParticleEngine::createEmitterContainerFromFile( const ConstString& _fileSystemName, const FilePath & _filename )
	{
		InputStreamInterface* file = 
            FILE_SERVICE(m_serviceProvider)->openInputFile( _fileSystemName, _filename );

		if( file == NULL )
		{
			LOGGER_ERROR(m_serviceProvider)( "ParticleEngine %s can't open file %s"				
				, _fileSystemName.c_str()
				, _filename.c_str() 
				);

			return 0;
		}

		int fileSize = file->size();

        TBlobject fileBuffer;
        fileBuffer.resize(fileSize);

		file->read( &fileBuffer[0], fileSize );

		file->destroy();

		EmitterContainerInterface * container = m_particleSystem->createEmitterContainerFromMemory( &fileBuffer[0] );

		if( container == NULL )
		{
			LOGGER_ERROR(m_serviceProvider)( "ParticleEngine can't create emitter container '%s'"
				, _filename.c_str() 
				);

			return 0;
		}

		return container;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEngine::flushEmitter( const mt::mat4f & _viewMatrix, EmitterInterface * _emitter, ParticleMesh * _meshes, ParticleVertices * _particles, size_t _particlesLimit, EmitterRenderFlush & _flush )
	{
		if( m_particleSystem->flushParticles( _viewMatrix, _emitter, _meshes, _particles, _particlesLimit, _flush ) == false )
        {
            return false;
        }

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEngine::releaseEmitterContainer( EmitterContainerInterface* _containerInterface )
	{
		m_particleSystem->releaseEmitterContainer( _containerInterface );
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
        size_t count = m_renderParticleNum - _count;

		return count;
	}
	//////////////////////////////////////////////////////////////////////////
}
