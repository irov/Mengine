#pragma once

#include "Interface/LoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class LoaderResourceJSON
        : public LoaderInterface
        , public Factorable
    {
    public:
        LoaderResourceJSON();
        ~LoaderResourceJSON() override;

    protected:
        bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}
