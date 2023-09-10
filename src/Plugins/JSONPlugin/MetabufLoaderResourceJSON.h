#pragma once

#include "Interface/MetabufLoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MetabufLoaderResourceJSON
        : public MetabufLoaderInterface
        , public Factorable
    {
    public:
        MetabufLoaderResourceJSON();
        ~MetabufLoaderResourceJSON() override;

    protected:
        bool load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta ) override;
    };
}
