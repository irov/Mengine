#pragma once

#include "Kernel/BasePrototypeGenerator.h"

namespace Mengine
{
    class EntityPrototypeGenerator
        : public BasePrototypeGenerator
    {
    public:
        EntityPrototypeGenerator();
        ~EntityPrototypeGenerator() override;

    protected:
        FactorablePointer generate( const Char * _doc ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<EntityPrototypeGenerator, PrototypeGeneratorInterface> EntityPrototypeGeneratorPtr;
    //////////////////////////////////////////////////////////////////////////
}