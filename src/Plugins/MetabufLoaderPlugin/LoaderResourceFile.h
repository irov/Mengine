#pragma once

#include "Interface/LoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class LoaderResourceFile
        : public LoaderInterface
        , public Factorable
    {
    public:
		LoaderResourceFile();
        ~LoaderResourceFile() override;

	protected:
		bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}
