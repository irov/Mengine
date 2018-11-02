#pragma once

#include "Interface/LoaderServiceInterface.h"

namespace Mengine
{    
    class LoaderResourceMusic
        : public LoaderInterface
    {
    public:
        LoaderResourceMusic();
        ~LoaderResourceMusic() override;

    protected:
        bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}
