#pragma once

#include "Interface/LoaderServiceInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class LoaderResourceImageSequence
        : public LoaderInterface
    {
    public:
		LoaderResourceImageSequence();
        ~LoaderResourceImageSequence() override;

	protected:
		bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}





