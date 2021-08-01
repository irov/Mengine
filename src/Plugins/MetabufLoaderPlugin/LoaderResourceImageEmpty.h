#pragma once

#include "Interface/LoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class LoaderResourceImageEmpty
        : public LoaderInterface
        , public Factorable
    {
    public:
        LoaderResourceImageEmpty();
        ~LoaderResourceImageEmpty() override;

    protected:
        bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}