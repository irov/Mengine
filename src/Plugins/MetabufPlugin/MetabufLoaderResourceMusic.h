#pragma once

#include "Interface/MetabufLoaderInterface.h"

#include "Engine/ResourceMusic.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MetabufLoaderResourceMusic
        : public MetabufLoaderInterface
        , public Factorable
    {
    public:
        MetabufLoaderResourceMusic();
        ~MetabufLoaderResourceMusic() override;

    protected:
        bool load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta ) override;
    };
}
