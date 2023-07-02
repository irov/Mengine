#pragma once

#include "Kernel/BasePrototypeGenerator.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ScenePrototypeGenerator
        : public BasePrototypeGenerator
    {
    public:
        ScenePrototypeGenerator();
        ~ScenePrototypeGenerator() override;

    protected:
        FactorablePointer generate( const DocumentInterfacePtr & _doc ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ScenePrototypeGenerator, PrototypeGeneratorInterface> ScenePrototypeGeneratorPtr;
    //////////////////////////////////////////////////////////////////////////
}