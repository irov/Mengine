#pragma once

#include "Interface/LoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class LoaderResourceSpineAtlasTexturepacker
        : public LoaderInterface
        , public Factorable
    {
    public:
        LoaderResourceSpineAtlasTexturepacker();
        ~LoaderResourceSpineAtlasTexturepacker() override;

    protected:
        bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}
