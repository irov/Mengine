#pragma once

#include "Interface/LoaderServiceInterface.h"

namespace Mengine
{
    class LoaderResourceCursorICO
        : public LoaderInterface
    {
    public:
		LoaderResourceCursorICO();
		~LoaderResourceCursorICO() override;

    protected:
		bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}