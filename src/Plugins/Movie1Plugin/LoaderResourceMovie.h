#pragma once

#include "Interface/MetabufLoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class LoaderResourceMovie
        : public MetabufLoaderInterface
        , public Factorable
    {
    public:
        LoaderResourceMovie();
        ~LoaderResourceMovie() override;

    protected:
        bool load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta ) override;
    };
}