#pragma once

#include "SpineInterface.h"

#include "Kernel/Resource.h"
#include "Kernel/Content.h"

#include "Plugins/TexturepackerPlugin/TexturepackerInterface.h"

#include "spine/spine.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceSpineAtlasTexturepacker
        : public Resource
        , public Content
    {
        DECLARE_VISITABLE( Resource );
        DECLARE_CONTENTABLE();

    public:
        ResourceSpineAtlasTexturepacker();
        ~ResourceSpineAtlasTexturepacker() override;

    public:
        void setResourceTexturepacker( const ConstString & _resourceTexturepackerName );
        const ConstString & getResourceTexturepacker() const;

    public:
        spAtlas * getAtlas() const;

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
