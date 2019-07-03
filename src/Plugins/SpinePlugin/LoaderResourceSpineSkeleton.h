#pragma once

#include "Interface/LoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class LoaderResourceSpineSkeleton
        : public LoaderInterface
        , public Factorable
    {
    public:
        LoaderResourceSpineSkeleton();
        ~LoaderResourceSpineSkeleton() override;

    protected:
        bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}
