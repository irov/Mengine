#pragma once

#include "Interface/MetabufLoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MetabufLoaderResourceCursorSystem
        : public MetabufLoaderInterface
        , public Factorable
    {
    public:
        MetabufLoaderResourceCursorSystem();
        ~MetabufLoaderResourceCursorSystem() override;

    protected:
        bool load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta ) override;
    };
}