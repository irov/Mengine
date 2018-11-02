#pragma once

#include "Interface/LoaderServiceInterface.h"

namespace Mengine
{
    class LoaderResourceSound
        : public LoaderInterface
    {
    public:
        LoaderResourceSound();
        ~LoaderResourceSound() override;

    protected:
        bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}
