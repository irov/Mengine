#include "ResourceSpineAtlasDefault.h"

#include "Interface/ResourceServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/MemoryHelper.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & _spAtlasPage_createTexture2( const ResourceSpineAtlasDefault * _resource, const char * _path )
    {
        const ResourceImagePtr & resourceImage = _resource->getResourceImage_( _path );

        return resourceImage;
    }
    //////////////////////////////////////////////////////////////////////////
    extern "C" void _spAtlasPage_createTexture( spAtlasPage * _page, const char * _path )
    {
        Mengine::ResourceSpineAtlasDefault * resource = (Mengine::ResourceSpineAtlasDefault *)_page->atlas->rendererObject;

        const ResourceImagePtr & resourceImage = _spAtlasPage_createTexture2( resource, _path );

        if( resourceImage == nullptr )
        {
            return;
        }

        const mt::vec2f & size = resourceImage->getMaxSize();

        _page->width = (int)size.x;
        _page->height = (int)size.y;
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
    void ResourceSpineAtlasDefault::addResourceImageDesc( const ConstString & _name, const ConstString & _resourceImageName )
    {
        ImageDesc desc;
        desc.name = _name;
        desc.resourceImageName = _resourceImageName;
        desc.resourceImage = nullptr;

        m_images.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    spAtlas * ResourceSpineAtlasDefault::getAtlas() const
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
            const ResourceImagePtr & resourceImage = RESOURCE_SERVICE()
                ->getResource( desc.resourceImageName );

            MENGINE_ASSERTION_MEMORY_PANIC( resourceImage, false, "'%s' category '%s' group '%s' invalid get image resource '%s'"
                , this->getName().c_str()
                , this->getFileGroup()->getName().c_str()
                , this->getGroupName().c_str()
                , desc.resourceImageName.c_str()
            );

            if( resourceImage->incrementReference() == false )
            {
                return false;
            }

            desc.resourceImage = resourceImage;
        }

        const FilePath & filePath = this->getFilePath();
        const FileGroupInterfacePtr & fileGroup = this->getFileGroup();

        MemoryInterfacePtr atlas_memory = Helper::createMemoryCacheFile( fileGroup, filePath, false, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( atlas_memory, false );

        const char * atlas_memory_buffer = atlas_memory->getBuffer();
        size_t atlas_memory_size = atlas_memory->getSize();

        spAtlas * atlas = spAtlas_create( atlas_memory_buffer, (int)atlas_memory_size, "", this );

        atlas_memory = nullptr;

        MENGINE_ASSERTION_MEMORY_PANIC( atlas, false );

        m_atlas = atlas;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceSpineAtlasDefault::_release()
    {
        for( ImageDesc & desc : m_images )
        {
            const ResourceImagePtr & resourceImage = desc.resourceImage;

            resourceImage->decrementReference();
            desc.resourceImage = nullptr;
        }

        if( m_atlas != nullptr )
        {
            spAtlas_dispose( m_atlas );
            m_atlas = nullptr;
        }
    }
}