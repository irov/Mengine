#pragma once

#include "Interface/LoaderServiceInterface.h"

namespace Mengine
{
    class LoaderResourceImageSolid
		: public LoaderInterface
    {
    public:
		LoaderResourceImageSolid();
        ~LoaderResourceImageSolid() override;

    protected:
		bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}
