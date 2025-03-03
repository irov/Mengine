#pragma once

#include "Interface/MetabufLoaderInterface.h"

#include "Kernel/ResourceImageSequence.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MetabufLoaderResourceImageSequence
        : public MetabufLoaderInterface
        , public Factorable
    {
    public:
        MetabufLoaderResourceImageSequence();
        ~MetabufLoaderResourceImageSequence() override;

    protected:
        bool load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta ) override;
    };
}





