#pragma once

#include "SpineInterface.h"

#include "ResourceSpineAtlas.h"

#include "Kernel/Resource.h"
#include "Kernel/Content.h"

#include "Plugins/TexturepackerPlugin/TexturepackerInterface.h"

#include "spine/spine.h"

namespace Mengine
{
    class ResourceSpineAtlasTexturepacker
        : public ResourceSpineAtlas
        , public UnknownResourceSpineAtlasTexturepacker
    {
        DECLARE_VISITABLE( ResourceSpineAtlas );
        DECLARE_UNKNOWABLE();

    public:
        ResourceSpineAtlasTexturepacker();
        ~ResourceSpineAtlasTexturepacker() override;

    public:
        void setResourceTexturepackerName( const ConstString & _resourceTexturepackerName ) override;
        const ConstString & getResourceTexturepackerName() const override;

    public:
        spAtlas * getSpineAtlas() const override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        ConstString m_resourceTexturepackerName;
        ResourcePtr m_resourceTexturepacker;

        spAtlas * m_atlas;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceSpineAtlasTexturepacker> ResourceSpineAtlasTexturepackerPtr;
    //////////////////////////////////////////////////////////////////////////
}
