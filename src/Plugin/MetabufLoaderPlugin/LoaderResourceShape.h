#pragma once

#include "Interface/LoaderServiceInterface.h"

namespace Mengine
{
    class LoaderResourceShape
        : public LoaderInterface
    {
    public:
        LoaderResourceShape();
        ~LoaderResourceShape() override;

    protected:
        bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}
