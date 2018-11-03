#pragma once

#include "Interface/LoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class LoaderResourceCursorSystem
        : public LoaderInterface
        , public Factorable
    {
    public:
		LoaderResourceCursorSystem();
		~LoaderResourceCursorSystem() override;

    protected:
		bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}