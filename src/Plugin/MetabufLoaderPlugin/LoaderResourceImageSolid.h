#pragma once

#include "Interface/LoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class LoaderResourceImageSolid
		: public LoaderInterface
        , public Factorable
    {
    public:
		LoaderResourceImageSolid();
        ~LoaderResourceImageSolid() override;

    protected:
		bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}
