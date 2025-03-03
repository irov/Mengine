#pragma once

#include "Interface/MetabufLoaderInterface.h"

#include "Engine/ResourceFile.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MetabufLoaderResourceFile
        : public MetabufLoaderInterface
        , public Factorable
    {
    public:
        MetabufLoaderResourceFile();
        ~MetabufLoaderResourceFile() override;

    protected:
        bool load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta ) override;
    };
}
