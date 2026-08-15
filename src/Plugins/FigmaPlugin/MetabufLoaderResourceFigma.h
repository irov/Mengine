#pragma once

#include "Interface/MetabufLoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MetabufLoaderResourceFigma
        : public MetabufLoaderInterface
        , public Factorable
    {
    public:
        MetabufLoaderResourceFigma();
        ~MetabufLoaderResourceFigma() override;

    protected:
        bool load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta ) override;
    };
}
