#pragma once

#include "Interface/LoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class LoaderResourceImageSubstract
        : public LoaderInterface
        , public Factorable
    {
    public:
        LoaderResourceImageSubstract();
        ~LoaderResourceImageSubstract() override;

    protected:
        bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}
