#pragma once

#include "Interface/MetabufLoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MetabufLoaderResourceMovie2
        : public MetabufLoaderInterface
        , public Factorable
    {
    public:
        MetabufLoaderResourceMovie2();
        ~MetabufLoaderResourceMovie2() override;

    protected:
        bool load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta ) override;
    };
}