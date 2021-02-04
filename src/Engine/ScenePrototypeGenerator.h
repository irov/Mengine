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
        FactorablePointer generate( const DocumentPtr & _doc ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ScenePrototypeGenerator, PrototypeGeneratorInterface> ScenePrototypeGeneratorPtr;
    //////////////////////////////////////////////////////////////////////////
}