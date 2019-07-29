#pragma once

#include "SpineInterface.h"

#include "ResourceSpineAtlas.h"

#include "Kernel/Resource.h"
#include "Kernel/Content.h"

#include "spine/spine.h"

namespace Mengine
{
    class ResourceSpineSkeleton
        : public Resource
        , public Content
        , public UnknownResourceSpineSkeleton
    {
        DECLARE_VISITABLE( Resource );
        DECLARE_UNKNOWABLE();
        DECLARE_CONTENTABLE();

    public:
        ResourceSpineSkeleton();
        ~ResourceSpineSkeleton() override;

    public:
        void setResourceSpineAtlasName( const ConstString & _resourceSpineAtlas ) override;
        const ConstString & getResourceSpineAtlasName() const override;

    public:
        spSkeletonData * getSkeletonData() const;
        spAnimation * findSkeletonAnimation( const ConstString & _name ) const;

    protected:
        bool _compile() override;
        void _release() override;

    public:
        ConstString m_resourceSpineAtlasName;
        ResourceSpineAtlasPtr m_resourceSpineAtlas;

        spSkeletonData * m_skeletonData;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceSpineSkeleton> ResourceSpineSkeletonPtr;
    //////////////////////////////////////////////////////////////////////////
}
