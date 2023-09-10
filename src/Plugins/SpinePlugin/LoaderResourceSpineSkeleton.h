#pragma once

#include "Interface/MetabufLoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class LoaderResourceSpineSkeleton
        : public MetabufLoaderInterface
        , public Factorable
    {
    public:
        LoaderResourceSpineSkeleton();
        ~LoaderResourceSpineSkeleton() override;

    protected:
        bool load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta ) override;
    };
}
