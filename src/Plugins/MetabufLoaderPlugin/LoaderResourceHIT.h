#pragma once

#include "Interface/LoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class LoaderResourceHIT
        : public LoaderInterface
        , public Factorable
    {
    public:
		LoaderResourceHIT();
        ~LoaderResourceHIT() override;

    public:
		bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}
