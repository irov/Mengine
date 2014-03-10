#	include "ParticleEngine.h"

#   include "Interface/FileSystemInterface.h"
#	include "Interface/CacheInterface.h"

#   include "Config/Blobject.h"

#	include "Logger/Logger.h"

#	include "Core/Viewport.h"

#	include "math/box2.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ParticleService, Menge::ParticleServiceInterface, Menge::ParticleEngine );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	ParticleEngine::ParticleEngine()
		: m_serviceProvider(nullptr)
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
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * ParticleEngine::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	ParticleEmitterContainerInterfacePtr ParticleEngine::createEmitterContainerFromFile( const ConstString& _fileGroupName, const FilePath & _filename )
	{
		InputStreamInterfacePtr file = FILE_SERVICE(m_serviceProvider)
			->openInputFile( _fileGroupName, _filename );
		
		if( file == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ParticleEngine %s can't open file %s"				
				, _fileGroupName.c_str()
				, _filename.c_str() 
				);

			return nullptr;
		}

		size_t fileSize = file->size();

		void * memory = nullptr;
		size_t bufferId = CACHE_SERVICE(m_serviceProvider)
			->lockBuffer( fileSize, &memory );

		unsigned char * buffer = static_cast<unsigned char *>(memory);

		file->read( buffer, fileSize );

		file = nullptr;

		ParticleEmitterContainerInterfacePtr container = PARTICLE_SYSTEM(m_serviceProvider)
            ->createEmitterContainerFromMemory( _filename, buffer );

		CACHE_SERVICE(m_serviceProvider)
			->unlockBuffer( bufferId );

		if( container == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ParticleEngine can't create emitter container '%s'"
				, _filename.c_str() 
				);

			return nullptr;
		}

		return container;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEngine::flushEmitter( const mt::mat4f & _viewMatrix, ParticleEmitterInterface * _emitter, ParticleMesh * _meshes, ParticleVertices * _particles, size_t _particlesLimit, ParticleEmitterRenderFlush & _flush )
	{
		bool successful = PARTICLE_SYSTEM(m_serviceProvider)
			->flushParticles( _viewMatrix, _emitter, _meshes, _particles, _particlesLimit, _flush );

        return successful;
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
	void ParticleEngine::update()
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
