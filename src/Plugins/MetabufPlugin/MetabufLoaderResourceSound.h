#pragma once

#include "Interface/MetabufLoaderInterface.h"

#include "Kernel/ResourceSound.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MetabufLoaderResourceSound
        : public MetabufLoaderInterface
        , public Factorable
    {
    public:
        MetabufLoaderResourceSound();
        ~MetabufLoaderResourceSound() override;

    protected:
        bool load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta ) override;
    };
}
