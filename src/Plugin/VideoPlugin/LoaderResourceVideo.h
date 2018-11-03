#pragma once

#include "Interface/LoaderServiceInterface.h"

namespace Mengine
{
    class LoaderResourceVideo
        : public LoaderInterface
    {
    public:
        LoaderResourceVideo();
        ~LoaderResourceVideo() override;

    protected:
        bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}
