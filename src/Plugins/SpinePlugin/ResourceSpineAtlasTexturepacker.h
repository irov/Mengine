#pragma once

#include "ResourceSpineAtlas.h"

#include "Kernel/Resource.h"
#include "Kernel/Content.h"

#include "Plugins/TexturepackerPlugin/TexturepackerInterface.h"

#include "spine/spine.h"

namespace Mengine
{
    class ResourceSpineAtlasTexturepacker
        : public ResourceSpineAtlas
    {
    public:
        ResourceSpineAtlasTexturepacker();
        ~ResourceSpineAtlasTexturepacker() override;

    public:
        void setResourceTexturepackerName( const ConstString & _resourceTexturepackerName );
        const ConstString & getResourceTexturepackerName() const;

    public:
        spAtlas * getAtlas() const override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        ConstString m_resourceTexturepackerName;
        ResourcePtr m_resourceTexturepacker;

        spAtlas * m_atlas;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<ResourceSpineAtlasTexturepacker> ResourceSpineAtlasTexturepackerPtr;
    //////////////////////////////////////////////////////////////////////////
}
