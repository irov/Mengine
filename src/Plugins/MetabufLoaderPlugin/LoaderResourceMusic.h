#pragma once

#include "Interface/LoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class LoaderResourceMusic
        : public LoaderInterface
        , public Factorable
    {
    public:
        LoaderResourceMusic();
        ~LoaderResourceMusic() override;

    protected:
        bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}
