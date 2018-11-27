#pragma once

#include "AstralaxInterface.h"

#include "Kernel/Resource.h"
#include "Kernel/Content.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/Polygon.h"

#include "Config/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<ResourceImagePtr> VectorResourceImages;
    //////////////////////////////////////////////////////////////////////////
    class ResourceAstralax
        : public Resource
        , public Content
    {
        DECLARE_VISITABLE( Resource );
        DECLARE_CONTENTABLE();
        DECLARE_MAGICABLE( MAGIC_PTZ );

    public:
        ResourceAstralax();
        ~ResourceAstralax() override;

    public:
        void setResourceImages( const VectorResourceImages & _resourceImages );
        uint32_t getResourceImageCount() const;
        const ResourceImagePtr & getResourceImage( uint32_t _index ) const;

    public:
        AstralaxEmitterInterfacePtr createEmitter();

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        VectorResourceImages m_resourceImages;

        AstralaxEmitterContainerInterfacePtr m_container;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ResourceAstralax, Resource> ResourceAstralaxPtr;
}
