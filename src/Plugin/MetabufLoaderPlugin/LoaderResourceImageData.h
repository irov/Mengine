#pragma once

#include "Interface/LoaderServiceInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class LoaderResourceImageData
        : public LoaderInterface
    {
    public:
		LoaderResourceImageData();
        ~LoaderResourceImageData() override;

    protected:
		bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}