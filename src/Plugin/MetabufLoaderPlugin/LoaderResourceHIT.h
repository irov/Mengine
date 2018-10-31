#pragma once

#include "Interface/LoaderServiceInterface.h"

namespace Mengine
{
    class LoaderResourceHIT
        : public LoaderInterface
    {
    public:
		LoaderResourceHIT();
        ~LoaderResourceHIT() override;

    public:
		bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}
