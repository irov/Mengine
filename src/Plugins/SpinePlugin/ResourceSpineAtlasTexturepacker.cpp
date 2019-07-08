#include "ResourceSpineAtlasTexturepacker.h"

#include "Interface/ResourceServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/MemoryHelper.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "spine/extension.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceSpineAtlasTexturepacker::ResourceSpineAtlasTexturepacker()
        : m_atlas( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceSpineAtlasTexturepacker::~ResourceSpineAtlasTexturepacker()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceSpineAtlasTexturepacker::setResourceTexturepackerName( const ConstString & _resourceTexturepackerName )
    {
        m_resourceTexturepackerName = _resourceTexturepackerName;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ResourceSpineAtlasTexturepacker::getResourceTexturepackerName() const
    {
        return m_resourceTexturepackerName;
    }
    //////////////////////////////////////////////////////////////////////////
    spAtlas * ResourceSpineAtlasTexturepacker::getAtlas() const
    {
        return m_atlas;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceSpineAtlasTexturepacker::_compile()
    {
        ResourcePtr resourceTexturepacker = RESOURCE_SERVICE()
            ->getResource( m_resourceTexturepackerName );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceTexturepacker, false );

        m_resourceTexturepacker = resourceTexturepacker;

        UnknownResourceTexturepackerInterface * unknownResourceTexturepacker = m_resourceTexturepacker->getUnknown();

        uint32_t atlasWidth = unknownResourceTexturepacker->getAtlasWidth();
        uint32_t atlasHeight = unknownResourceTexturepacker->getAtlasHeight();

        float atlasWidthF = (float)atlasWidth;
        float atlasHeightF = (float)atlasHeight;

        float atlasWidthInv = unknownResourceTexturepacker->getAtlasWidthInv();
        float atlasHeightInv = unknownResourceTexturepacker->getAtlasHeightInv();

        spAtlas * atlas = NEW( spAtlas );

        MENGINE_ASSERTION_MEMORY_PANIC( atlas, false );

        char * atlas_name = MALLOC( char, 1 );
        atlas_name[0] = '\0';

        spAtlasPage * page = spAtlasPage_create( atlas, atlas_name );

        page->width = atlasWidth;
        page->height = atlasHeight;
        page->minFilter = SP_ATLAS_LINEAR;
        page->magFilter = SP_ATLAS_LINEAR;
        page->uWrap = SP_ATLAS_CLAMPTOEDGE;
        page->vWrap = SP_ATLAS_CLAMPTOEDGE;
        page->rendererObject = nullptr;

        atlas->pages = page;

        spAtlasRegion * lastRegion = nullptr;

        const VectorResourceImages & frames = unknownResourceTexturepacker->getFrames();

        for( const ResourceImagePtr & frame : frames )
        {
            spAtlasRegion * region = spAtlasRegion_create();

            region->page = page;

            const ConstString & frame_name = frame->getName();

            ConstString::size_type frame_name_size = frame_name.size();

            char * region_name = MALLOC( char, frame_name_size + 1 );
            memcpy( region_name, frame_name.c_str(), frame_name_size );
            region_name[frame_name_size] = '\0';

            region->name = region_name;

            bool uvImageRotate = frame->isUVImageRotate();

            region->rotate = uvImageRotate;

            const mt::uv4f & uv = frame->getUVImage();

            region->x = (int)(uv.p0.x * atlasWidthF + 0.5f);
            region->y = (int)(uv.p0.y * atlasHeightF + 0.5f);

            const mt::vec2f & size = frame->getSize();

            region->width = (int)(size.x + 0.5f);
            region->height = (int)(size.y + 0.5f);

            region->u = uv.p0.x;
            region->v = uv.p0.y;

            if( uvImageRotate == true )
            {
                region->u2 = (region->x + region->height) * atlasWidthInv;
                region->v2 = (region->y + region->width) * atlasHeightInv;
            }
            else
            {
                region->u2 = (region->x + region->width) * atlasWidthInv;
                region->v2 = (region->y + region->height) * atlasHeightInv;
            }

            const mt::vec2f & maxSize = frame->getMaxSize();

            region->originalWidth = (int)(maxSize.x + 0.5f);
            region->originalHeight = (int)(maxSize.y + 0.5f);

            const mt::vec2f & offset = frame->getOffset();

            region->offsetX = (int)(offset.x + 0.5f);
            region->offsetY = (int)(offset.y + 0.5f);

            region->index = -1;

            if( lastRegion != nullptr )
            {
                lastRegion->next = region;
            }
            else
            {
                atlas->regions = region;
            }

            lastRegion = region;
        }

        m_atlas = atlas;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceSpineAtlasTexturepacker::_release()
    {
        if( m_resourceTexturepacker != nullptr )
        {
            m_resourceTexturepacker->decrementReference();
            m_resourceTexturepacker = nullptr;
        }

        if( m_atlas != nullptr )
        {
            spAtlas_dispose( m_atlas );
            m_atlas = nullptr;
        }
    }
}