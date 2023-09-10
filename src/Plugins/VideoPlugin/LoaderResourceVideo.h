#pragma once

#include "Interface/MetabufLoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class LoaderResourceVideo
        : public MetabufLoaderInterface
        , public Factorable
    {
    public:
        LoaderResourceVideo();
        ~LoaderResourceVideo() override;

    protected:
        bool load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta ) override;
    };
}
