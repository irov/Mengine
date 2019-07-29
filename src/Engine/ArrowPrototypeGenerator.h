#pragma once

#include "Kernel/BasePrototypeGenerator.h"

namespace Mengine
{
    class ArrowPrototypeGenerator
        : public BasePrototypeGenerator
    {
    public:
        ArrowPrototypeGenerator();
        ~ArrowPrototypeGenerator() override;

    protected:
        FactorablePointer generate( const Char * _doc ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ArrowPrototypeGenerator, PrototypeGeneratorInterface> ArrowPrototypeGeneratorPtr;
    //////////////////////////////////////////////////////////////////////////
}