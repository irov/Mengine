#pragma once

#include "Interface/LoaderServiceInterface.h"

namespace Mengine
{
    class LoaderResourceImageDefault
        : public LoaderInterface
    {
    public:
		LoaderResourceImageDefault();
		~LoaderResourceImageDefault() override;

	protected:
		bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}