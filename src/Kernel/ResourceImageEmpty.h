#pragma once

#include "Kernel/ResourceImage.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceImageEmpty
        : public ResourceImage
    {
        DECLARE_FACTORABLE( ResourceImageEmpty );
        DECLARE_VISITABLE( ResourceImage );

    public:
        ResourceImageEmpty();
        ~ResourceImageEmpty() override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        void prepareImageFrame_();
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceImageEmpty> ResourceImageEmptyPtr;
    //////////////////////////////////////////////////////////////////////////
}