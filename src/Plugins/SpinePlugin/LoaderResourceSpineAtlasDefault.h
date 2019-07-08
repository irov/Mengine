#pragma once

#include "Interface/LoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class LoaderResourceSpineAtlasDefault
        : public LoaderInterface
        , public Factorable
    {
    public:
        LoaderResourceSpineAtlasDefault();
        ~LoaderResourceSpineAtlasDefault() override;

    protected:
        bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}
