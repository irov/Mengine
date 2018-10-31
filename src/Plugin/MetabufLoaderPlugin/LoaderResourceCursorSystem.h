#pragma once

#include "Interface/LoaderServiceInterface.h"

namespace Mengine
{
    class LoaderResourceCursorSystem
        : public LoaderInterface
    {
    public:
		LoaderResourceCursorSystem();
		~LoaderResourceCursorSystem() override;

    protected:
		bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}