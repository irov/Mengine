#pragma once

#include "Kernel/ResourceImage.h"

#include "Config/Typedef.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceImageSubstract
        : public ResourceImage
    {
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

    public:
        void correctUVTexture();

    protected:
        ResourceImagePtr m_resourceImage;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceImageSubstract> ResourceImageSubstractPtr;
    //////////////////////////////////////////////////////////////////////////
}
