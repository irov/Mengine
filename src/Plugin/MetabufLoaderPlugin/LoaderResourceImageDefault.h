#pragma once

#include "Interface/LoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class LoaderResourceImageDefault
        : public LoaderInterface
        , public Factorable
    {
    public:
		LoaderResourceImageDefault();
		~LoaderResourceImageDefault() override;

	protected:
		bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}