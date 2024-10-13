#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/ThreadGuard.h"

#include "Config/Typedef.h"
#include "Config/Lambda.h"
#include "Config/Atomic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Compilable
        : public Mixin
    {
    public:
        Compilable();
        ~Compilable() override;

    public:
        MENGINE_INLINE bool isCompile() const;

    public:
        virtual bool compile();
        virtual void release();

    public:
        typedef Lambda<bool()> LambdaRecompile;
        bool recompile( const LambdaRecompile & _lambda );

    protected:
        virtual bool _compile();
        virtual void _release();

        virtual void _recompile();
        virtual void _uncompile();

    private:
        bool m_compile;

        MENGINE_THREAD_GUARD_INIT( Compilable );
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Compilable> CompilablePtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool Compilable::isCompile() const
    {
        return m_compile;
    }
    //////////////////////////////////////////////////////////////////////////
}
