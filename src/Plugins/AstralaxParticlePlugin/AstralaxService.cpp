#include "AstralaxService.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

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
    AstralaxEmitterContainerInterfacePtr AstralaxService::createEmitterContainerFromFile( const FileGroupInterfacePtr& _fileGroup, const FilePath & _fileName, const Char * _doc )
    {
        if( this->isAvailableService() == false )
        {
            LOGGER_ERROR( "Particle Engine not available" );

            return nullptr;
        }

        AstralaxEmitterContainerInterfacePtr container = ASTRALAX_SYSTEM()
            ->createEmitterContainerFromMemory( _fileGroup, _fileName, m_archivator, _doc );

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
