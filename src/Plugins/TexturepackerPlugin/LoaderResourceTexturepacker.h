#pragma once

#include "Interface/MetabufLoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class LoaderResourceTexturepacker
        : public MetabufLoaderInterface
        , public Factorable
    {
    public:
        LoaderResourceTexturepacker();
        ~LoaderResourceTexturepacker() override;

    protected:
        bool load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta ) override;
    };
}
