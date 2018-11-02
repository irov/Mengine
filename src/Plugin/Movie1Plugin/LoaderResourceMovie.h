#pragma once

#include "Interface/LoaderServiceInterface.h"

namespace Mengine
{    
    class LoaderResourceMovie
        : public LoaderInterface
    {
    public:
        LoaderResourceMovie();
        ~LoaderResourceMovie() override;

    protected:
        bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}