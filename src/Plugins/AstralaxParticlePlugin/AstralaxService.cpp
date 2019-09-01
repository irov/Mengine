#include "AstralaxService.h"

#include "Interface/ConfigServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Viewport.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

#include "math/box2.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AstralaxService, Mengine::AstralaxService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    AstralaxService::AstralaxService()
        : m_maxParticlesNum( 10000U )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AstralaxService::~AstralaxService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxService::_availableService() const
    {
        bool available = CONFIG_VALUE( "Engine", "AstralaxAvailable", true );

        if( available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxService::_initializeService()
    {
        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( archivator, false );

        m_archivator = archivator;

        m_maxParticlesNum = CONFIG_VALUE( "Engine", "ParticleMaxCount", 10000U );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxService::_finalizeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AstralaxEmitterContainerInterfacePtr AstralaxService::createEmitterContainerFromFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const Char * _doc )
    {
        if( this->isAvailableService() == false )
        {
            LOGGER_ERROR( "Particle Engine not available" );

            return nullptr;
        }

        AstralaxEmitterContainerInterfacePtr container = ASTRALAX_SYSTEM()
            ->createEmitterContainerFromMemory( _fileGroup, _filePath, m_archivator, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( container, nullptr, "can't create emitter container '%s:%s'"
            , _fileGroup->getName().c_str()
            , _filePath.c_str()
        );

        return container;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AstralaxService::getMaxParticlesCount() const
    {
        return m_maxParticlesNum;
    }
    //////////////////////////////////////////////////////////////////////////
}
