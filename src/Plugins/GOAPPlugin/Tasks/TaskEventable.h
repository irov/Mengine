#pragma once

#include "GOAP/TaskInterface.h"

#include "Interface/EventationInterface.h"

#include "Kernel/Eventable.h"

#include "Config/Lambda.h"

namespace Mengine
{
    template<class T>
    struct ClassOf;

    template<class R, class C, class ... Args>
    struct ClassOf<R( C:: * )(Args ...)>
    {
        typedef R result_type;
        typedef C class_type;
    };

    template<class R, class C, class ... Args>
    struct ClassOf<R( C:: * )(Args ...) const>
    {
        typedef R result_type;
        typedef C class_type;
    };

#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__ANDROID__) || defined(__ANDROID__) || defined(__APPLE__) || defined(__linux__)
    template<class R, class C, class ... Args>
    struct ClassOf<R( C:: * )(Args ...) noexcept>
    {
        typedef R result_type;
        typedef C class_type;
    };

    template<class R, class C, class ... Args>
    struct ClassOf<R( C:: * )(Args ...) const noexcept>
    {
        typedef R result_type;
        typedef C class_type;
    };
#endif

    class TaskEventable
        : public GOAP::TaskInterface
    {
    public:
        template<class M, class ... Args>
        TaskEventable( GOAP::Allocator * _allocator, Eventable * _eventable, uint32_t _event, M _method, Args && ... _args )
            : GOAP::TaskInterface( _allocator )
            , m_eventable( _eventable )
            , m_event( _event )
            , m_invoke( [&, _method, _args ...]( EventReceiverInterface * _receiver )
        {
            typename ClassOf<M>::class_type * t = static_cast<typename ClassOf<M>::class_type *>(_receiver);

            (t->*_method)(_args ...);
        } )
        {
        }

        ~TaskEventable() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;

    protected:
        Eventable * m_eventable;
        uint32_t m_event;
        Lambda<void( EventReceiverInterface * )> m_invoke;
    };
}