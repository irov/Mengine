#include "SpriteCache.h"

#include "Interface/ConfigServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/CodecServiceInterface.h"

#include "ResourceImageDefault.h"

#include "Kernel/FileStreamHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Content.h"
#include "Kernel/Document.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SpriteCache::SpriteCache()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SpriteCache::~SpriteCache()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SpriteCache::initialize()
    {
        uint32_t SpriteCacheReserve = CONFIG_VALUE( "Engine", "SpriteCacheReserve", 256U );

        m_resources.reserve( SpriteCacheReserve );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SpriteCache::finalize()
    {
        m_resources.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & SpriteCache::getResourceImage( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _codecType, const mt::vec2f & _maxSize )
    {
        CacheKey key{ _fileGroup->getName(), _filePath };

        const ResourceImagePtr & cacheResourceImage = m_resources.find( key );

        if( cacheResourceImage != nullptr )
        {
            return cacheResourceImage;
        }

        ResourceImageDefaultPtr resourceImage = RESOURCE_SERVICE()
            ->generateResource( STRINGIZE_STRING_LOCAL( "ResourceImageDefault" ), MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceImage, ResourceImagePtr::none() );

        Content * content = resourceImage->getContent();

        MENGINE_ASSERTION_MEMORY_PANIC( content, ResourceImagePtr::none() );

        content->setFileGroup( _fileGroup );
        content->setFilePath( _filePath );

        if( _codecType.empty() == true )
        {
            const ConstString & codecType = CODEC_SERVICE()
                ->findCodecType( _filePath );

            content->setCodecType( codecType );
        }
        else
        {
            content->setCodecType( _codecType );
        }

        if( _maxSize.x < 0.f || _maxSize.y < 0.f )
        {
            const ConstString & codecType = content->getCodecType();

            InputStreamInterfacePtr stream = Helper::openInputStreamFile( _fileGroup, _filePath, false, MENGINE_DOCUMENT_FUNCTION );

            MENGINE_ASSERTION_MEMORY_PANIC( stream, ResourceImagePtr::none() );

            ImageDecoderInterfacePtr decoder = CODEC_SERVICE()
                ->createDecoderT<ImageDecoderInterfacePtr>( codecType, MENGINE_DOCUMENT_FUNCTION );

            MENGINE_ASSERTION_MEMORY_PANIC( decoder, ResourceImagePtr::none() );

            if( decoder->prepareData( stream ) == false )
            {
                return ResourceImagePtr::none();
            }

            const ImageCodecDataInfo * dataInfo = decoder->getCodecDataInfo();

            mt::vec2f maxSize( (float)dataInfo->width, (float)dataInfo->height );

            resourceImage->setMaxSize( maxSize );
            resourceImage->setSize( maxSize );
        }
        else
        {
            resourceImage->setMaxSize( _maxSize );
            resourceImage->setSize( _maxSize );
        }

        resourceImage->correctUVTexture();

        const ResourceImagePtr & storeResourceImage = m_resources.emplace( key, resourceImage );

        return storeResourceImage;
    }
}