#include "AstralaxService.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/FileSystemInterface.h"
#include "Interface/ArchiveServiceInterface.h"

#include "Kernel/Logger.h"

#include "Kernel/Viewport.h"

#include "math/box2.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AstralaxService, Mengine::AstralaxService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    AstralaxService::AstralaxService()
        : m_maxParticlesNum( 10000U )
        , m_available( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AstralaxService::~AstralaxService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxService::_initializeService()
    {
        m_available = CONFIG_VALUE( "Engine", "AstralaxAvaliable", true );

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
    void AstralaxService::_finalizeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxService::isAvailable() const
    {
        return m_available;
    }
    //////////////////////////////////////////////////////////////////////////
    AstralaxEmitterContainerInterfacePtr AstralaxService::createEmitterContainerFromFile( const FileGroupInterfacePtr& _fileGroup, const FilePath & _fileName, const ConstString & _whoName )
    {
        if( m_available == false )
        {
            LOGGER_ERROR( "Particle Engine not available" );

            return nullptr;
        }

        InputStreamInterfacePtr stream = FILE_SERVICE()
            ->openInputFile( _fileGroup, _fileName, false );

        if( stream == nullptr )
        {
            LOGGER_ERROR( "can't open file %s:%s"
                , _fileGroup->getName().c_str()
                , _fileName.c_str()
            );

            return nullptr;
        }

        AstralaxEmitterContainerInterfacePtr container = ASTRALAX_SYSTEM()
            ->createEmitterContainerFromMemory( stream, m_archivator, _whoName );

        if( container == nullptr )
        {
            LOGGER_ERROR( "can't create emitter container '%s:%s'"
                , _fileGroup->getName().c_str()
                , _fileName.c_str()
            );

            return nullptr;
        }

        return container;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AstralaxService::getMaxParticlesCount() const
    {
        return m_maxParticlesNum;
    }
    //////////////////////////////////////////////////////////////////////////
}
