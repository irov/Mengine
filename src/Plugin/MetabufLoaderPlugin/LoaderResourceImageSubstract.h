#pragma once

#include "Interface/LoaderServiceInterface.h"

namespace Mengine
{
    class LoaderResourceImageSubstract
        : public LoaderInterface
    {
    public:
        LoaderResourceImageSubstract();
        ~LoaderResourceImageSubstract() override;

    protected:
        bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}
