#pragma once

#include "Interface/MetabufLoaderInterface.h"

#include "Engine/ResourceShape.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MetabufLoaderResourceShape
        : public MetabufLoaderInterface
        , public Factorable
    {
    public:
        MetabufLoaderResourceShape();
        ~MetabufLoaderResourceShape() override;

    protected:
        bool load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta ) override;
    };
}
