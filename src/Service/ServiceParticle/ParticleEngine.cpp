#include "ParticleEngine.h"

#include "Interface/StringizeInterface.h"
#include "Interface/ConfigInterface.h"
#include "Interface/FileSystemInterface.h"

#include "Kernel/Logger.h"

#include "Kernel/Viewport.h"

#include "math/box2.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ParticleService, Mengine::ParticleEngine2 );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    ParticleEngine2::ParticleEngine2()
        : m_maxParticlesNum( 10000U )
        , m_available( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ParticleEngine2::~ParticleEngine2()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ParticleEngine2::_initializeService()
    {
        m_available = CONFIG_VALUE( "Engine", "ParticleService2Avaliable", true );

        //m_available = false;

        if( SERVICE_EXIST( Mengine::ParticleSystemInterface2 ) == false )
        {
            m_available = false;
        }

        if( m_available == false )
        {
            return true;
        }

        const ArchivatorInterfacePtr & archivator = ARCHIVE_SERVICE()
            ->getArchivator( STRINGIZE_STRING_LOCAL( "lz4" ) );

        if( archivator == nullptr )
        {
            return false;
        }

        m_archivator = archivator;

        m_maxParticlesNum = CONFIG_VALUE( "Engine", "ParticleMaxCount", 10000U );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ParticleEngine2::_finalizeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ParticleEngine2::isAvailable() const
    {
        return m_available;
    }
    //////////////////////////////////////////////////////////////////////////
    ParticleEmitterContainerInterface2Ptr ParticleEngine2::createEmitterContainerFromFile( const FileGroupInterfacePtr& _fileGroup, const FilePath & _fileName, const ConstString & _whoName )
    {
        if( m_available == false )
        {
            LOGGER_ERROR( "ParticleEngine2::createEmitterContainerFromFile Particle Engine not available"
            );

            return nullptr;
        }

        InputStreamInterfacePtr stream = FILE_SERVICE()
            ->openInputFile( _fileGroup, _fileName, false );

        if( stream == nullptr )
        {
            LOGGER_ERROR( "ParticleEngine2::createEmitterContainerFromFile can't open file %s:%s"
                , _fileGroup->getName().c_str()
                , _fileName.c_str()
            );

            return nullptr;
        }

        ParticleEmitterContainerInterface2Ptr container = PARTICLE_SYSTEM2()
            ->createEmitterContainerFromMemory( stream, m_archivator, _whoName );

        if( container == nullptr )
        {
            LOGGER_ERROR( "ParticleEngine2::createEmitterContainerFromFile can't create emitter container '%s:%s'"
                , _fileGroup->getName().c_str()
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
