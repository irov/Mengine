#pragma once

#include "Interface/MetabufLoaderInterface.h"

#include "Kernel/ResourceImageDefault.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MetabufLoaderResourceImageDefault
        : public MetabufLoaderInterface
        , public Factorable
    {
    public:
        MetabufLoaderResourceImageDefault();
        ~MetabufLoaderResourceImageDefault() override;

    protected:
        bool load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta ) override;
    };
}