#pragma once

#include "Interface/LoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class LoaderResourceShape
        : public LoaderInterface
        , public Factorable
    {
    public:
        LoaderResourceShape();
        ~LoaderResourceShape() override;

    protected:
        bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}
