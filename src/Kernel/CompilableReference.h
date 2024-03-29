#pragma once

#include "Kernel/Compilable.h"
#include "Kernel/ReferenceCounter.h"
#include "Kernel/ThreadGuard.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class CompilableReference
        : public Compilable
    {
    public:
        CompilableReference();
        ~CompilableReference() override;

    public:
        MENGINE_INLINE uint32_t getCompileReferenceCount() const;

    public:
        bool compile() override;
        void release() override;

    private:
        ReferenceCounter m_compileReferenceCount;

        MENGINE_THREAD_GUARD_INIT( CompilableReference );
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<CompilableReference> CompilableReferencePtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE uint32_t CompilableReference::getCompileReferenceCount() const
    {
        uint32_t count = m_compileReferenceCount.getReferenceCount();

        return count;
    }
    //////////////////////////////////////////////////////////////////////////
}
