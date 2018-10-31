#pragma once

#include "Interface/LoaderServiceInterface.h"

namespace Mengine
{
    class LoaderResourceFile
        : public LoaderInterface
    {
    public:
		LoaderResourceFile();
        ~LoaderResourceFile() override;

	protected:
		bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}
