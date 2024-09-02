#include "ResourceSpineAtlasDefault.h"

#include "Kernel/Logger.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & _spAtlasPage_createTexture2( const ResourceSpineAtlasDefault * _resource, const char * _filePath )
    {
        const ResourceImagePtr & resourceImage = _resource->getResourceImage_( _filePath );

        return resourceImage;
    }
    //////////////////////////////////////////////////////////////////////////
    extern "C" void _spAtlasPage_createTexture( spAtlasPage * _page, const char * _filePath )
    {
        Mengine::ResourceSpineAtlasDefault * resource = (Mengine::ResourceSpineAtlasDefault *)_page->atlas->rendererObject;

        const ResourceImagePtr & resourceImage = _spAtlasPage_createTexture2( resource, _filePath );

        if( resourceImage == nullptr )
        {
            return;
        }

        const mt::vec2f & maxSize = resourceImage->getMaxSize();

        _page->width = (int)maxSize.x;
        _page->height = (int)maxSize.y;
        _page->rendererObject = (void *)resourceImage.get();
    }
    //////////////////////////////////////////////////////////////////////////
    extern "C" void _spAtlasPage_disposeTexture( spAtlasPage * _page )
    {
        MENGINE_UNUSED( _page );
    }
    //////////////////////////////////////////////////////////////////////////
    extern "C" char * _spUtil_readFile( const char * path, int * length )
    {
        MENGINE_UNUSED( path );
        MENGINE_UNUSED( length );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceSpineAtlasDefault::ResourceSpineAtlasDefault()
        : m_atlas( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceSpineAtlasDefault::~ResourceSpineAtlasDefault()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceSpineAtlasDefault::addResourceImageDesc( const ConstString & _name, const ResourceImagePtr & _resourceImage )
    {
        ImageDesc desc;
        desc.name = _name;
        desc.resourceImage = _resourceImage;

        m_images.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    spAtlas * ResourceSpineAtlasDefault::getSpineAtlas() const
    {
        return m_atlas;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & ResourceSpineAtlasDefault::getResourceImage_( const Char * _name ) const
    {
        for( const ImageDesc & desc : m_images )
        {
            if( desc.name != _name )
            {
                continue;
            }

            return desc.resourceImage;
        }

        return ResourceImagePtr::none();
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceSpineAtlasDefault::_compile()
    {
        for( ImageDesc & desc : m_images )
        {
            const ResourceImagePtr & resourceImage = desc.resourceImage;

            if( resourceImage->compile() == false )
            {
                return false;
            }
        }

        const ContentInterfacePtr & content = this->getContent();

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
        const FilePath & filePath = content->getFilePath();

        MemoryInterfacePtr atlas_memory = Helper::createMemoryCacheFile( fileGroup, filePath, false, false, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ASSERTION_MEMORY_PANIC( atlas_memory );

        const char * atlas_memory_buffer = atlas_memory->getBuffer();
        size_t atlas_memory_size = atlas_memory->getSize();

        spAtlas * atlas = spAtlas_create( atlas_memory_buffer, (int)atlas_memory_size, "", this );
        MENGINE_ASSERTION_MEMORY_PANIC( atlas );

        atlas_memory = nullptr;

        m_atlas = atlas;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceSpineAtlasDefault::_release()
    {
        for( ImageDesc & desc : m_images )
        {
            const ResourceImagePtr & resourceImage = desc.resourceImage;

            resourceImage->release();
        }

        if( m_atlas != nullptr )
        {
            spAtlas_dispose( m_atlas );
            m_atlas = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}