#pragma once

#include "Interface/LoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class LoaderResourceCursorICO
        : public LoaderInterface
        , public Factorable
    {
    public:
        LoaderResourceCursorICO();
        ~LoaderResourceCursorICO() override;

    protected:
        bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}