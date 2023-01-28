#pragma once

#include "Interface/ThreadIdentityInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/String.h"
#include "Kernel/Document.h"

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
        DECLARE_FACTORABLE( SDLFileOutputStream );

    public:
        SDLThreadIdentity();
        ~SDLThreadIdentity() override;

    public:
        bool initialize( EThreadPriority _priority, const ConstString & _name, const DocumentPtr & _doc );
        void finalize();

    public:
        ThreadIdentityRunnerInterfacePtr run( const LambdaThreadRunner & _lambda ) override;

    public:
        void main();

    public:
        ThreadId getThreadId() const override;

    public:
        void join() override;
        void cancel() override;

    public:
        bool isCurrentThread() const override;

    public:
        EThreadPriority getPriority() const;

    protected:
        EThreadPriority m_priority;
        ConstString m_name;

        ThreadIdentityRunnerInterfacePtr m_runner;

        SDL_threadID m_threadId;

        SDL_Thread * m_thread;

#ifdef MENGINE_DEBUG
        DocumentPtr m_doc;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDLThreadIdentity, ThreadIdentityInterface> SDLThreadIdentityPtr;
    //////////////////////////////////////////////////////////////////////////
}
