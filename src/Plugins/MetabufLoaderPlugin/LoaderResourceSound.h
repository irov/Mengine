#pragma once

#include "Interface/LoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class LoaderResourceSound
        : public LoaderInterface
        , public Factorable
    {
    public:
        LoaderResourceSound();
        ~LoaderResourceSound() override;

    protected:
        bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}
