#pragma once

#include "Interface/LoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{    
    class LoaderResourceMovie
        : public LoaderInterface
        , public Factorable
    {
    public:
        LoaderResourceMovie();
        ~LoaderResourceMovie() override;

    protected:
        bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}