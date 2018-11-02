#pragma once

#include "Interface/LoaderServiceInterface.h"

namespace Mengine
{    
    class LoaderResourceWindow
        : public LoaderInterface
    {
    public:
        LoaderResourceWindow();
        ~LoaderResourceWindow() override;

    protected:
        bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}
