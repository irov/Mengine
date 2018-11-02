#pragma once

#include "ResourceImageDefault.h"

#include "Config/Typedef.h"

namespace Mengine
{
    class ResourceImageSubstract
        : public ResourceImage
    {
        DECLARE_VISITABLE( ResourceImage );

    public:
        ResourceImageSubstract();
        ~ResourceImageSubstract() override;

    public:
        void setResourceImageName( const ConstString & _resourceImageName );
        const ConstString & getResourceImageName() const;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        ConstString m_resourceImageName;
        ResourceImagePtr m_resourceImage;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ResourceImageSubstract> ResourceImageSubstractPtr;
    //////////////////////////////////////////////////////////////////////////
}
