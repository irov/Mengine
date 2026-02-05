#pragma once

#include "Interface/ScriptEmbeddableInterface.h"
#include "Interface/DocumentInterface.h"

#include "Kernel/FactorableUnique.h"

namespace Mengine
{
    class DummyScriptEmbedded
        : public ScriptEmbeddableInterface
    {
    public:
        DummyScriptEmbedded();
        ~DummyScriptEmbedded() override;

    protected:
        const ConstString & getEmbeddedType() const override;
        ScriptEmbeddingInterfacePtr generateEmbedding( const DocumentInterfacePtr & _doc ) const override;
    };

    template<class T>
    class BaseScriptEmbedded
        : public ScriptEmbeddableInterface
    {
    protected:
        const ConstString & getEmbeddedType() const override
        {
            return T::getFactorableType();
        }

        ScriptEmbeddingInterfacePtr generateEmbedding( const DocumentInterfacePtr & _doc ) const override
        {
            ScriptEmbeddingInterfacePtr embedding = Helper::makeFactorableUnique<T>( _doc );

            return embedding;
        }
    };
}

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   define EXTEND_EMBEDDABLE(Type) Mengine::BaseScriptEmbedded<Type>
#else 
#   define EXTEND_EMBEDDABLE(Type) Mengine::DummyScriptEmbedded
#endif