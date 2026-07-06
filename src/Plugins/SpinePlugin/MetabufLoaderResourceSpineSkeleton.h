#pragma once

#include "Interface/MetabufLoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MetabufLoaderResourceSpineSkeleton
        : public MetabufLoaderInterface
        , public Factorable
    {
    public:
        MetabufLoaderResourceSpineSkeleton();
        ~MetabufLoaderResourceSpineSkeleton() override;

    protected:
        bool load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta ) override;
    };
}
