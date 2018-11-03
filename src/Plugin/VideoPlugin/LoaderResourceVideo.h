#pragma once

#include "Interface/LoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class LoaderResourceVideo
        : public LoaderInterface
        , public Factorable
    {
    public:
        LoaderResourceVideo();
        ~LoaderResourceVideo() override;

    protected:
        bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}
