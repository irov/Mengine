#pragma once

#include "Interface/MetabufLoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MetabufLoaderResourceVideo
        : public MetabufLoaderInterface
        , public Factorable
    {
    public:
        MetabufLoaderResourceVideo();
        ~MetabufLoaderResourceVideo() override;

    protected:
        bool load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta ) override;
    };
}
