#include "ResourceSpineAtlasTexturepacker.h"

#include "Interface/ResourceServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Config/StdString.h"

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
    void ResourceSpineAtlasTexturepacker::addResourceTexturepackerName( const ConstString & _resourceTexturepackerName )
    {
        TexturepackerDesc desc;
        desc.resourceTexturepackerName = _resourceTexturepackerName;
        desc.resourceTexturepacker = nullptr;

        m_texturepackers.push_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    spAtlas * ResourceSpineAtlasTexturepacker::getSpineAtlas() const
    {
        return m_atlas;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceSpineAtlasTexturepacker::_compile()
    {
        ResourceBankInterface * resourceBank = this->getResourceBank();

        spAtlas * atlas = NEW( spAtlas );

        MENGINE_ASSERTION_MEMORY_PANIC( atlas );

        spAtlasPage * lastPage = nullptr;
        spAtlasRegion * lastRegion = nullptr;

        for( TexturepackerDesc & desc : m_texturepackers )
        {
            char * page_name = MALLOC( char, 1 );
            page_name[0] = '\0';

            spAtlasPage * page = spAtlasPage_create( atlas, page_name );

            if( lastPage != nullptr )
            {
                lastPage->next = page;
            }
            else
            {
                atlas->pages = page;
            }

            lastPage = page;

            ResourcePtr resourceTexturepacker = resourceBank->getResource( desc.resourceTexturepackerName );

            MENGINE_ASSERTION_MEMORY_PANIC( resourceTexturepacker );

            UnknownResourceTexturepackerInterface * unknownResourceTexturepacker = resourceTexturepacker->getUnknown();

            uint32_t atlasWidth = unknownResourceTexturepacker->getAtlasWidth();
            uint32_t atlasHeight = unknownResourceTexturepacker->getAtlasHeight();

            float atlasWidthF = (float)atlasWidth;
            float atlasHeightF = (float)atlasHeight;

            float atlasWidthInv = unknownResourceTexturepacker->getAtlasWidthInv();
            float atlasHeightInv = unknownResourceTexturepacker->getAtlasHeightInv();

            page->width = atlasWidth;
            page->height = atlasHeight;
            page->minFilter = SP_ATLAS_LINEAR;
            page->magFilter = SP_ATLAS_LINEAR;
            page->uWrap = SP_ATLAS_CLAMPTOEDGE;
            page->vWrap = SP_ATLAS_CLAMPTOEDGE;

            const ResourceImagePtr & atlasImage = unknownResourceTexturepacker->getAtlasImage();
            page->rendererObject = atlasImage.get();

            const VectorResourceImages & frames = unknownResourceTexturepacker->getFrames();

            for( const ResourceImagePtr & frame : frames )
            {
                spAtlasRegion * region = spAtlasRegion_create();

                region->page = page;

                const ConstString & frame_name = frame->getName();

                ConstString::size_type frame_name_size = frame_name.size();

                char * region_name = MALLOC( char, frame_name_size + 1 );
                MENGINE_MEMCPY( region_name, frame_name.c_str(), frame_name_size );
                region_name[frame_name_size] = '\0';

                region->name = region_name;

                bool uvImageRotate = frame->isUVImageRotate();

                region->rotate = uvImageRotate;

                region->flip = 0; //TODO

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

                region->offsetX = (int)(maxSize.x - size.x - offset.x + 0.5f);
                region->offsetY = (int)(maxSize.y - size.y - offset.y + 0.5f);

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

            desc.resourceTexturepacker = resourceTexturepacker;
        }

        m_atlas = atlas;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceSpineAtlasTexturepacker::_release()
    {
        for( TexturepackerDesc & desc : m_texturepackers )
        {
            desc.resourceTexturepacker->release();
            desc.resourceTexturepacker = nullptr;
        }

        if( m_atlas != nullptr )
        {
            spAtlas_dispose( m_atlas );
            m_atlas = nullptr;
        }
    }
}