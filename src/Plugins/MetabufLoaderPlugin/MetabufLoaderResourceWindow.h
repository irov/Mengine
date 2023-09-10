#pragma once

#include "Interface/MetabufLoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MetabufLoaderResourceWindow
        : public MetabufLoaderInterface
        , public Factorable
    {
    public:
        MetabufLoaderResourceWindow();
        ~MetabufLoaderResourceWindow() override;

    protected:
        bool load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta ) override;
    };
}
