#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ScriptEmbeddableInterface
        : public Mixin
    {
    public:
        virtual const ConstString & getEmbeddedType() const = 0;
        virtual ScriptEmbeddingInterfacePtr generateEmbedding( const DocumentInterfacePtr & _doc ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ScriptEmbeddableInterface> ScriptEmbeddableInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}