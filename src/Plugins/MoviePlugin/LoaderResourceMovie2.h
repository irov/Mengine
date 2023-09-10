#pragma once

#include "Interface/MetabufLoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class LoaderResourceMovie2
        : public MetabufLoaderInterface
        , public Factorable
    {
    public:
        LoaderResourceMovie2();
        ~LoaderResourceMovie2() override;

    protected:
        bool load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta ) override;
    };
}