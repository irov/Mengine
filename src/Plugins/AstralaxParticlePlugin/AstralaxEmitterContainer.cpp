#include "AstralaxEmitterContainer.h"
#include "AstralaxParticleSystem.h"

#include "Interface/MemoryInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/PrefetcherServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"

#include "Kernel/String.h"
#include "Kernel/Stream.h"

#include "Kernel/ResourceImage.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/FactoryPoolWithListener.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AstralaxEmitterContainer::AstralaxEmitterContainer()
        : m_mf( 0 )
        , m_dublicateCount( 0 )
        , m_ptcId( 0 )
    {
    };
    //////////////////////////////////////////////////////////////////////////
    AstralaxEmitterContainer::~AstralaxEmitterContainer()
    {
        MENGINE_ASSERTION( m_mf == 0, "astralax container is not finalized" );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitterContainer::initialize( const FileGroupInterfacePtr& _fileGroup, const FilePath & _fileName, const ArchivatorInterfacePtr & _archivator )
    {
        MemoryInterfacePtr memory;
        if( PREFETCHER_SERVICE()
            ->getStream( _fileGroup, _fileName, memory ) == false )
        {
            InputStreamInterfacePtr stream = FILE_SERVICE()
                ->openInputFile( _fileGroup, _fileName, false, MENGINE_DOCUMENT_FUNCTION );

            if( stream == nullptr )
            {
                LOGGER_ERROR( "can't open file %s:%s"
                    , _fileGroup->getName().c_str()
                    , _fileName.c_str()
                );

                return false;
            }

            memory = Helper::loadStreamArchiveMagicMemory( stream, _archivator, GET_MAGIC_NUMBER( MAGIC_PTZ ), GET_MAGIC_VERSION( MAGIC_PTZ ), MENGINE_DOCUMENT_FUNCTION );
        }

        if( memory == nullptr )
        {
            LOGGER_ERROR( "invalid get data"
            );

            return false;
        }

        const void * binary_memory = memory->getBuffer();
        size_t binary_size = memory->getSize();

        HM_FILE mf;
        if( this->loadContainer_( binary_memory, binary_size, mf ) == false )
        {
            return false;
        }

        m_mf = mf;

        m_memory = memory;

        int32_t atlasCount = Magic_GetStaticAtlasCount( m_mf );

        if( atlasCount > 0 )
        {
            MAGIC_STATIC_ATLAS atlas;
            Magic_GetStaticAtlas( m_mf, 0, &atlas );

            m_ptcId = atlas.ptc_id;
        }

        m_resourceImages.resize( atlasCount );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitterContainer::finalize()
    {
        MENGINE_ASSERTION( m_dublicateCount == 0, "astralax container has not deleted emitters" );

        Magic_CloseFile( m_mf );
        m_mf = 0;

        m_memory = nullptr;

        m_resourceImages.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitterContainer::isValid() const
    {
        if( Magic_HasTextures( m_mf ) == true )
        {
            LOGGER_ERROR( "particle textures are stored within the file"
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AstralaxEmitterContainer::getPtcId() const
    {
        return m_ptcId;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitterContainer::setAtlasResourceImage( uint32_t _index, const ResourceImagePtr & _resourceImage )
    {
        if( _index >= m_resourceImages.size() )
        {
            LOGGER_ERROR( "invalid add resource image '%s' index '%d' atlas count '%d'"
                , _resourceImage->getName().c_str()
                , _index
                , m_resourceImages.size()
            );

            return false;
        }

        m_resourceImages[_index] = _resourceImage;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & AstralaxEmitterContainer::getAtlasResourceImage( const Char * _file ) const
    {
        int32_t atlasCount = Magic_GetStaticAtlasCount( m_mf );

        for( int32_t i = 0; i != atlasCount; ++i )
        {
            MAGIC_STATIC_ATLAS atlas;
            Magic_GetStaticAtlas( m_mf, i, &atlas );

            if( strcmp( atlas.file, _file ) != 0 )
            {
                continue;
            }

            const ResourceImagePtr & image = m_resourceImages[i];

            return image;
        }

        LOGGER_ERROR( "not found atlas %s"
            , _file
        );

        return ResourceImagePtr::none();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitterContainer::loadContainer_( const void * _buffer, size_t _size, HM_FILE & _mf ) const
    {
        MENGINE_UNUSED( _size );

        HM_FILE mf = Magic_OpenFileInMemory( reinterpret_cast<const char *>(_buffer) );

        if( mf == MAGIC_ERROR )
        {
            LOGGER_ERROR( "invalid open file in memory (alredy open)"
            );

            return false;
        }

        if( mf == MAGIC_UNKNOWN )
        {
            LOGGER_ERROR( "invalid open file in memory (invalid format or version)"
            );

            return false;
        }

        _mf = mf;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    HM_EMITTER AstralaxEmitterContainer::initialEmitterId() const
    {
        MAGIC_FIND_DATA find;
        const char * magicName = Magic_FindFirst( m_mf, &find, MAGIC_FOLDER | MAGIC_EMITTER );

        while( magicName != nullptr )
        {
            if( find.animate == 1 )
            {
                HM_EMITTER id = 0;

                try
                {
                    id = Magic_LoadEmitter( m_mf, magicName );
                }
                catch( const std::exception & _ex )
                {
                    LOGGER_ERROR( "invalid load emitter %s (catch exception '%s')"
                        , magicName
                        , _ex.what()
                    );

                    return 0;
                }

                if( id == 0 )
                {
                    LOGGER_ERROR( "invalid load emitter %s"
                        , magicName
                    );

                    return 0;
                }

                return id;
            }
            else
            {
                const char * currentFolder = Magic_GetCurrentFolder( m_mf );
                Magic_SetCurrentFolder( m_mf, magicName );

                HM_EMITTER id = this->initialEmitterId();

                Magic_SetCurrentFolder( m_mf, currentFolder );

                if( id != 0 )
                {
                    return id;
                }
            }

            magicName = Magic_FindNext( m_mf, &find );
        }

        LOGGER_ERROR( "not found emitter"
        );

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    HM_EMITTER AstralaxEmitterContainer::createEmitterId()
    {
        HM_EMITTER emitterId = this->initialEmitterId();

        if( emitterId == 0 )
        {
            return 0;
        }

        ++m_dublicateCount;

        return emitterId;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitterContainer::destroyEmitterId( HM_EMITTER _id )
    {
        if( _id == 0 )
        {
            return;
        }

        --m_dublicateCount;

        Magic_UnloadEmitter( _id );
    }
}