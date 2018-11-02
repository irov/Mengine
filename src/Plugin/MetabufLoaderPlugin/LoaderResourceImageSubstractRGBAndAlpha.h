#pragma once

#include "Interface/LoaderServiceInterface.h"

namespace Mengine
{
    class LoaderResourceImageSubstractRGBAndAlpha
        : public LoaderInterface
    {
    public:
        LoaderResourceImageSubstractRGBAndAlpha();
        ~LoaderResourceImageSubstractRGBAndAlpha() override;

    protected:
        bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}