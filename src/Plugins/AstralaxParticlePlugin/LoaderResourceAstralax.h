#pragma once

#include "Interface/LoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class LoaderResourceAstralax
        : public LoaderInterface
        , public Factorable
    {
    public:
        LoaderResourceAstralax();
        ~LoaderResourceAstralax() override;

    protected:
        bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}
