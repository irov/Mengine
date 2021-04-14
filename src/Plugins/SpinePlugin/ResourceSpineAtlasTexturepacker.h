#pragma once

#include "SpineInterface.h"

#include "ResourceSpineAtlas.h"

#include "Kernel/Resource.h"

#include "Plugins/TexturepackerPlugin/TexturepackerInterface.h"

#include "spine/spine.h"

namespace Mengine
{
    class ResourceSpineAtlasTexturepacker
        : public ResourceSpineAtlas
        , public UnknownResourceSpineAtlasTexturepackerInterface
    {
        DECLARE_FACTORABLE( ResourceSpineAtlasTexturepacker );
        DECLARE_VISITABLE( ResourceSpineAtlas );
        DECLARE_UNKNOWABLE();

    public:
        ResourceSpineAtlasTexturepacker();
        ~ResourceSpineAtlasTexturepacker() override;

    public:
        void addResourceTexturepacker( const ResourcePtr & _resourceTexturepacker ) override;

    public:
        spAtlas * getSpineAtlas() const override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        struct TexturepackerDesc
        {
            ResourcePtr resourceTexturepacker;
        };

        typedef Vector<TexturepackerDesc> VectorTexturepackers;
        VectorTexturepackers m_texturepackers;

        spAtlas * m_atlas;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceSpineAtlasTexturepacker> ResourceSpineAtlasTexturepackerPtr;
    //////////////////////////////////////////////////////////////////////////
}
