#	include "ParticleEngine2.h"

#   include "Interface/FileSystemInterface.h"
#   include "Interface/StringizeInterface.h"
#   include "Interface/ConfigInterface.h"

#	include "Logger/Logger.h"

#	include "Core/Viewport.h"

#	include "math/box2.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ParticleService, Menge::ParticleEngine2 );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	ParticleEngine2::ParticleEngine2()
		: m_maxParticlesNum(10000U)
		, m_available(true)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ParticleEngine2::~ParticleEngine2()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEngine2::_initialize()
	{
		m_available = CONFIG_VALUE( m_serviceProvider, "Engine", "ParticleService2Avaliable", true );

		//m_available = false;

		if( SERVICE_EXIST( m_serviceProvider, Menge::ParticleSystemInterface2 ) == false )
		{
			m_available = false;
		}

		if( m_available == false )
		{
			return true;
		}

		m_archivator = ARCHIVE_SERVICE(m_serviceProvider)
			->getArchivator( STRINGIZE_STRING_LOCAL(m_serviceProvider, "lz4") );

		if( m_archivator == nullptr )
		{
			return false;
		}

		m_maxParticlesNum = CONFIG_VALUE( m_serviceProvider, "Engine", "ParticleMaxCount", 10000U );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleEngine2::_finalize()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleEngine2::isAvailable() const
	{
		return m_available;
	}
	//////////////////////////////////////////////////////////////////////////
	ParticleEmitterContainerInterface2Ptr ParticleEngine2::createEmitterContainerFromFile( const ConstString& _fileGroupName, const FilePath & _fileName )
	{
		if( m_available == false )
		{
			return nullptr;
		}
		
		InputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
			->openInputFile( _fileGroupName, _fileName, false );
		
		if( stream == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ParticleEngine2::createEmitterContainerFromFile can't open file %s:%s"
				, _fileGroupName.c_str()
				, _fileName.c_str() 
				);

			return nullptr;
		}

		ParticleEmitterContainerInterface2Ptr container = PARTICLE_SYSTEM2(m_serviceProvider)
            ->createEmitterContainerFromMemory( stream, m_archivator );

		if( container == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ParticleEngine2::createEmitterContainerFromFile can't create emitter container '%s'"
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
}
