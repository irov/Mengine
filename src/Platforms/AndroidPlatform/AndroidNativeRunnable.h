#pragma once

#include "Kernel/Factorable.h"
#include "Kernel/IntrusiveLinked.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidNativeRunnable
        : public Factorable
        , public IntrusiveLinked<AndroidNativeRunnable>
    {
        DECLARE_FACTORABLE( AndroidNativeRunnable );

    public:
        typedef Lambda<void()> LambdaCallback;

    public:
        explicit AndroidNativeRunnable( const LambdaCallback & _callback );
        ~AndroidNativeRunnable() override;

    public:
        void invoke();
        void release();

    protected:
        LambdaCallback m_callback;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidNativeRunnable> AndroidNativeRunnablePtr;
    //////////////////////////////////////////////////////////////////////////
}
