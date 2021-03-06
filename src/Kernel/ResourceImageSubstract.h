#pragma once

#include "Kernel/ResourceImage.h"

#include "Config/Typedef.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceImageSubstract
        : public ResourceImage
    {
        DECLARE_FACTORABLE( ResourceImageSubstract );
        DECLARE_VISITABLE( ResourceImage );

    public:
        ResourceImageSubstract();
        ~ResourceImageSubstract() override;

    public:
        void setResourceImage( const ResourceImagePtr & _resourceImageName );
        const ResourceImagePtr & getResourceImage() const;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        bool _initialize() override;
        void _finalize() override;

    public:
        void correctUVTexture();

    protected:
        ResourceImagePtr m_resourceImage;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceImageSubstract> ResourceImageSubstractPtr;
    //////////////////////////////////////////////////////////////////////////
}
