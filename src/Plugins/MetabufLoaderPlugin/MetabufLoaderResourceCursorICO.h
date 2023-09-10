#pragma once

#include "Interface/MetabufLoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MetabufLoaderResourceCursorICO
        : public MetabufLoaderInterface
        , public Factorable
    {
    public:
        MetabufLoaderResourceCursorICO();
        ~MetabufLoaderResourceCursorICO() override;

    protected:
        bool load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta ) override;
    };
}