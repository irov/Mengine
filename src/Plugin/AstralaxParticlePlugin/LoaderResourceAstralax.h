#pragma once

#include "Interface/LoaderServiceInterface.h"

namespace Mengine
{
    class LoaderResourceAstralax
        : public LoaderInterface
    {
    public:
        LoaderResourceAstralax();
        ~LoaderResourceAstralax() override;

    protected:
        bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}
