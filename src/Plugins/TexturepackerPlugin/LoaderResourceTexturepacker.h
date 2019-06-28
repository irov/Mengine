#pragma once

#include "Interface/LoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class LoaderResourceTexturepacker
        : public LoaderInterface
        , public Factorable
    {
    public:
        LoaderResourceTexturepacker();
        ~LoaderResourceTexturepacker() override;

    protected:
        bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}
