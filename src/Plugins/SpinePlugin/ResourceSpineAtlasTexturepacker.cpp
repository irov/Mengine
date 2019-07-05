#include "ResourceSpineAtlasTexturepacker.h"

#include "Interface/ResourceServiceInterface.h"

#include "Metacode/Metacode.h"

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
    void ResourceSpineAtlasTexturepacker::setResourceTexturepacker( const ConstString & _resourceTexturepackerName )
    {
        m_resourceTexturepackerName = _resourceTexturepackerName;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ResourceSpineAtlasTexturepacker::getResourceTexturepacker() const
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

        spAtlas * atlas = NEW( spAtlas );

        //spAtlas * atlas = spAtlas_create( atlas_memory_buffer, (int)atlas_memory_size, "", this );

        //atlas_memory = nullptr;

        MENGINE_ASSERTION_MEMORY_PANIC( atlas, false );

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