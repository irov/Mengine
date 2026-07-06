#pragma once

#include "Interface/MetabufLoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MetabufLoaderResourceTexturepacker
        : public MetabufLoaderInterface
        , public Factorable
    {
    public:
        MetabufLoaderResourceTexturepacker();
        ~MetabufLoaderResourceTexturepacker() override;

    protected:
        bool load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta ) override;
    };
}
