#pragma once

#include "Kernel/BasePrototypeGenerator.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ArrowPrototypeGenerator
        : public BasePrototypeGenerator
    {
    public:
        ArrowPrototypeGenerator();
        ~ArrowPrototypeGenerator() override;

    protected:
        FactorablePointer generate( const DocumentInterfacePtr & _doc ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ArrowPrototypeGenerator, PrototypeGeneratorInterface> ArrowPrototypeGeneratorPtr;
    //////////////////////////////////////////////////////////////////////////
}