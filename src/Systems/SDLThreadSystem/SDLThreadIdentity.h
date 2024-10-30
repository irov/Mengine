#pragma once

#include "Interface/ThreadIdentityInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/String.h"

#include "Config/Atomic.h"

#include "SDL_thread.h"
#include "SDL_timer.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDLThreadIdentity
        : public ThreadIdentityInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( SDLThreadIdentity );

    public:
        SDLThreadIdentity();
        ~SDLThreadIdentity() override;

    public:
        bool initialize( const ThreadDescription & _description, EThreadPriority _priority );
        void finalize();

    public:
        ThreadIdentityRunnerInterfacePtr run( const LambdaThreadRunner & _lambda, Timestamp _sleep, const DocumentInterfacePtr & _doc ) override;

    public:
        const ThreadIdentityRunnerInterfacePtr & getRunner() const override;

    public:
        void main();

    public:
        ThreadId getThreadId() const override;

    public:
        void join( bool _cancel ) override;

    public:
        bool isCurrentThread() const override;

    public:
        EThreadPriority getPriority() const override;
        const ThreadDescription & getDescription() const override;

    protected:
        EThreadPriority m_priority;
        ThreadDescription m_description;

        ThreadIdentityRunnerInterfacePtr m_runner;

        SDL_threadID m_threadId;

        SDL_Thread * m_thread;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDLThreadIdentity, ThreadIdentityInterface> SDLThreadIdentityPtr;
    //////////////////////////////////////////////////////////////////////////
}
