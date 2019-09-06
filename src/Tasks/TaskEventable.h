#pragma once

#include "Interface/EventationInterface.h"

#include "Kernel/Eventable.h"

#include "Config/Lambda.h"

#include "GOAP/Task.h"

namespace Mengine
{
    template<class T>
    struct ClassOf;

    template<class R, class C, class ... Args>
    struct ClassOf<R (C::*)( Args ... )>
    {
        typedef R result_type;
        typedef C class_type;
    };

    class TaskEventable
        : public GOAP::Task
    {
    public:
        template<class M, class ... Args>
        TaskEventable( Eventable * _eventable, uint32_t _event, M _method, Args && ... _args )
            : m_eventable( _eventable )
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
        bool _onRun() override;
        void _onFinally() override;

    protected:
        Eventable * m_eventable;
        uint32_t m_event;
        Lambda<void( EventReceiverInterface * )> m_invoke;
    };
}