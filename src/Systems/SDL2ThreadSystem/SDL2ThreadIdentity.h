#pragma once

#include "Interface/ThreadIdentityInterface.h"

#include "Environment/SDL2/SDL2Includer.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/String.h"

#include "Config/Atomic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDL2ThreadIdentity
        : public ThreadIdentityInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( SDL2ThreadIdentity );

    public:
        SDL2ThreadIdentity();
        ~SDL2ThreadIdentity() override;

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
    typedef IntrusivePtr<SDL2ThreadIdentity, ThreadIdentityInterface> SDL2ThreadIdentityPtr;
    //////////////////////////////////////////////////////////////////////////
}
