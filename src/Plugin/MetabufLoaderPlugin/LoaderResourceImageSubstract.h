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

    protected:
        bool _loader( const Metabuf::Metadata * _parser ) override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        ConstString m_resourceImageName;
        ResourceImagePtr m_resourceImage;
    };
}
