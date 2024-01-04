#pragma once

#include "Interface/MetabufLoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MetabufLoaderResourceHIT
        : public MetabufLoaderInterface
        , public Factorable
    {
    public:
        MetabufLoaderResourceHIT();
        ~MetabufLoaderResourceHIT() override;

    public:
        bool load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta ) override;
    };
}
