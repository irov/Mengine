#pragma once

#include "Interface/MetabufLoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MetabufLoaderResourceImageData
        : public MetabufLoaderInterface
        , public Factorable
    {
    public:
        MetabufLoaderResourceImageData();
        ~MetabufLoaderResourceImageData() override;

    protected:
        bool load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta ) override;
    };
}