#pragma once

#include "Interface/MetabufLoaderInterface.h"

#include "Kernel/ResourceImageSubstract.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MetabufLoaderResourceImageSubstract
        : public MetabufLoaderInterface
        , public Factorable
    {
    public:
        MetabufLoaderResourceImageSubstract();
        ~MetabufLoaderResourceImageSubstract() override;

    protected:
        bool load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta ) override;
    };
}
