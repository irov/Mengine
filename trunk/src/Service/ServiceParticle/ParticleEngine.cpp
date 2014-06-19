#	include "ParticleEngine.h"

#   include "Interface/FileSystemInterface.h"

#   include "Config/Blobject.h"

#	include "Logger/Logger.h"

#	include "Core/Viewport.h"
#	include "Core/CacheMemoryBuffer.h"

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
	ParticleEmitterContainerInterfacePtr ParticleEngine::createEmitterContainerFromFile( const ConstString& _fileGroupName, const FilePath & _fileName )
	{
		InputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
			->openInputFile( _fileGroupName, _fileName );
		
		if( stream == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ParticleEngine %s can't open file %s"				
				, _fileGroupName.c_str()
				, _fileName.c_str() 
				);

			return nullptr;
		}
		
		ParticleEmitterContainerInterfacePtr container = PARTICLE_SYSTEM(m_serviceProvider)
            ->createEmitterContainerFromMemory( _fileName, stream );

		if( container == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ParticleEngine can't create emitter container '%s'"
				, _fileName.c_str() 
				);

			return nullptr;
		}

		return container;
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
