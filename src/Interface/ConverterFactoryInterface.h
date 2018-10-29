#pragma once

#include "Config/Typedef.h"

#include "Interface/ConverterInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ConverterFactoryInterface
        : public Mixin
    {
    public:
        virtual bool initialize() = 0;

    public:
        virtual ConverterInterfacePtr createConverter() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ConverterFactoryInterface> ConverterFactoryInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
