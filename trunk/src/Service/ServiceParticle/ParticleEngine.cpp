#	include "ParticleEngine.h"

#   include "Interface/FileSystemInterface.h"

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
	ParticleEmitterContainerInterfacePtr ParticleEngine::createParticleEmitterContainerFromFile( const ConstString& _fileGroupName, const FilePath & _fileName )
	{
		InputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
			->openInputFile( _fileGroupName, _fileName, false );
		
		if( stream == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ParticleEngine %s can't open file %s"				
				, _fileGroupName.c_str()
				, _fileName.c_str() 
				);

			return nullptr;
		}
		
		ParticleEmitterContainerInterfacePtr container = PARTICLE_SYSTEM(m_serviceProvider)
            ->createParticleEmitterContainer();

		if( container == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ParticleEngine::createEmitterContainerFromFile can't create emitter container '%s'"
				, _fileName.c_str() 
				);

			return nullptr;
		}

		if( PARTICLE_SYSTEM(m_serviceProvider)
			->loadParticleEmitterContainerFromMemory( container, stream ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ParticleEngine::createEmitterContainerFromFile can't load emitter container '%s'"
				, _fileName.c_str() 
				);

			return nullptr;
		}

		return container;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t ParticleEngine::getMaxParticlesCount() const
	{
		return m_maxParticlesNum;
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEngine::setMaxParticlesCount( uint32_t _count )
	{
		m_maxParticlesNum = _count;
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEngine::update()
	{
		m_renderParticleNum = m_maxParticlesNum;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t ParticleEngine::renderParticlesCount( uint32_t _count )
	{
        uint32_t count = m_renderParticleNum - _count;

		return count;
	}
	//////////////////////////////////////////////////////////////////////////
}
