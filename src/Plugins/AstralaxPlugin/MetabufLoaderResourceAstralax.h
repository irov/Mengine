#pragma once

#include "Interface/MetabufLoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MetabufLoaderResourceAstralax
        : public MetabufLoaderInterface
        , public Factorable
    {
    public:
        MetabufLoaderResourceAstralax();
        ~MetabufLoaderResourceAstralax() override;

    protected:
        bool load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta ) override;
    };
}
