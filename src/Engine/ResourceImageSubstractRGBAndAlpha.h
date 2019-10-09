#pragma once

#include "Kernel/ResourceImage.h"

#include "Config/Typedef.h"

namespace Mengine
{
    class ResourceImageSubstractRGBAndAlpha
        : public ResourceImage
    {
        DECLARE_VISITABLE( ResourceImage );

    public:
        ResourceImageSubstractRGBAndAlpha();
        ~ResourceImageSubstractRGBAndAlpha() override;

    public:
        void setResourceImageRGBName( const ConstString & _resourceImageRGBName );
        const ConstString & getResourceRGBName() const;

        void setResourceImageAlphaName( const ConstString & _resourceImageAlphaName );
        const ConstString & getResourceAlphaName() const;

    protected:
        bool _compile() override;
        void _release() override;

    public:
        void correctUVTexture();

    protected:
        ConstString m_resourceImageRGBName;
        ResourceImagePtr m_resourceImageRGB;

        ConstString m_resourceImageAlphaName;
        ResourceImagePtr m_resourceImageAlpha;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceImageSubstractRGBAndAlpha> ResourceImageSubstractRGBAndAlphaPtr;
    //////////////////////////////////////////////////////////////////////////
}