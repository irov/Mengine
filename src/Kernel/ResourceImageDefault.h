#pragma once

#include "Kernel/ResourceImage.h"

#include "Kernel/BaseContent.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceImageDefault
        : public ResourceImage
        , protected BaseContent
    {
        DECLARE_VISITABLE( ResourceImage );
        DECLARE_CONTENTABLE();

    public:
        ResourceImageDefault();
        ~ResourceImageDefault() override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        bool _initialize() override;
        void _finalize() override;

    public:
        void correctUVTexture();

    protected:
        void prepareImageFrame_();

#ifndef MENGINE_MASTER_RELEASE
    protected:
        bool m_forcePremultiply;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceImageDefault> ResourceImageDefaultPtr;
    //////////////////////////////////////////////////////////////////////////
}