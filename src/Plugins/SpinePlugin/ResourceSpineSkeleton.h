#pragma once

#include "SpineInterface.h"

#include "ResourceSpineAtlas.h"

#include "Kernel/Resource.h"
#include "Kernel/BaseContent.h"

#include "spine/spine.h"

namespace Mengine
{
    class ResourceSpineSkeleton
        : public Resource
        , private BaseContent
        , public UnknownResourceSpineSkeletonInterface
    {
        DECLARE_VISITABLE( Resource );
        DECLARE_UNKNOWABLE();
        DECLARE_CONTENTABLE();

    public:
        ResourceSpineSkeleton();
        ~ResourceSpineSkeleton() override;

    public:
        void setResourceSpineAtlas( const ResourcePtr & _resourceSpineAtlas ) override;
        const ResourceSpineAtlasPtr & getResourceSpineAtlas() const override;

    public:
        spSkeletonData * getSkeletonData() const;
        spAnimation * findSkeletonAnimation( const ConstString & _name ) const;

    protected:
        bool _compile() override;
        void _release() override;

    public:
        ResourceSpineAtlasPtr m_resourceSpineAtlas;

        spSkeletonData * m_skeletonData;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceSpineSkeleton> ResourceSpineSkeletonPtr;
    //////////////////////////////////////////////////////////////////////////
}
