#pragma once

#include "Interface/Interface.h"

#include "Interface/ConverterInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ConverterFactoryInterface
        : public Interface
    {
    public:
        virtual bool initialize() = 0;

    public:
        virtual ConverterInterfacePtr createConverter( const DocumentPtr & _doc ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ConverterFactoryInterface> ConverterFactoryInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
