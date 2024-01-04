#pragma once

#include "Interface/MetabufLoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MetabufLoaderResourceImageSubstractRGBAndAlpha
        : public MetabufLoaderInterface
        , public Factorable
    {
    public:
        MetabufLoaderResourceImageSubstractRGBAndAlpha();
        ~MetabufLoaderResourceImageSubstractRGBAndAlpha() override;

    protected:
        bool load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta ) override;
    };
}