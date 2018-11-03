#pragma once

#include "Interface/LoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class LoaderResourceImageSequence
        : public LoaderInterface
        , public Factorable
    {
    public:
		LoaderResourceImageSequence();
        ~LoaderResourceImageSequence() override;

	protected:
		bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}





