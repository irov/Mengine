#	include "ParticleEngine2.h"

#   include "Interface/FileSystemInterface.h"
#   include "Interface/StringizeInterface.h"

#   include "Config/Blobject.h"

#	include "Logger/Logger.h"

#	include "Core/Viewport.h"

#	include "math/box2.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ParticleService2, Menge::ParticleServiceInterface2, Menge::ParticleEngine2 );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	ParticleEngine2::ParticleEngine2()
		: m_serviceProvider(nullptr)
		, m_maxParticlesNum(2000)
		, m_renderParticleNum(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ParticleEngine2::~ParticleEngine2()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void ParticleEngine2::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * ParticleEngine2::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEngine2::initialize()
	{
		m_archivator = ARCHIVE_SERVICE(m_serviceProvider)
			->getArchivator( CONST_STRING_LOCAL(m_serviceProvider, "lz4") );

		if( m_archivator == nullptr )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEngine2::finalize()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	ParticleEmitterContainerInterface2Ptr ParticleEngine2::createEmitterContainerFromFile( const ConstString& _fileGroupName, const FilePath & _fileName )
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
		
		ParticleEmitterContainerInterface2Ptr container = PARTICLE_SYSTEM2(m_serviceProvider)
            ->createEmitterContainerFromMemory( stream, m_archivator );

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
	uint32_t ParticleEngine2::getMaxParticlesCount() const
	{
		return m_maxParticlesNum;
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEngine2::setMaxParticlesCount( uint32_t _count )
	{
		m_maxParticlesNum = _count;
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEngine2::update()
	{
		m_renderParticleNum = m_maxParticlesNum;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t ParticleEngine2::renderParticlesCount( uint32_t _count )
	{
        uint32_t count = m_renderParticleNum - _count;

		return count;
	}
	//////////////////////////////////////////////////////////////////////////
}
