#pragma once

#include "Kernel/Compilable.h"

namespace Mengine
{
    class CompilableReference
        : public Compilable
    {
    public:
        CompilableReference();
        ~CompilableReference() override;

    public:
        MENGINE_INLINE uint32_t getCompileReference() const;

    public:
        bool compile() override;
        void release() override;

    private:
        uint32_t m_compileReference;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<CompilableReference> CompilableReferencePtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE uint32_t CompilableReference::getCompileReference() const
    {
        return m_compileReference;
    }
    //////////////////////////////////////////////////////////////////////////
}
