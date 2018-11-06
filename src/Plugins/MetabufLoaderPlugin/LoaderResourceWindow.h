#pragma once

#include "Interface/LoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{    
    class LoaderResourceWindow
        : public LoaderInterface
        , public Factorable
    {
    public:
        LoaderResourceWindow();
        ~LoaderResourceWindow() override;

    protected:
        bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}
