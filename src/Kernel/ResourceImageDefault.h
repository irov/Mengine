#pragma once

#include "Kernel/ResourceImage.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceImageDefault
        : public ResourceImage
    {
        DECLARE_FACTORABLE( ResourceImageDefault );
        DECLARE_VISITABLE( ResourceImage );

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

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
    protected:
        bool m_forcePremultiply;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceImageDefault> ResourceImageDefaultPtr;
    //////////////////////////////////////////////////////////////////////////
}