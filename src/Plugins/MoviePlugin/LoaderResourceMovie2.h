#pragma once

#include "Interface/LoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class LoaderResourceMovie2
        : public LoaderInterface
        , public Factorable
    {
    public:
        LoaderResourceMovie2();
        ~LoaderResourceMovie2() override;

    protected:
        bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}