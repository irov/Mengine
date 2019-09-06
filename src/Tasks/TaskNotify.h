#pragma once

#include "Kernel/Node.h"

#include "GOAP/Task.h"

namespace Mengine
{
    template<class ID, class ... Args>
    class TaskNotify
        : public GOAP::Task
    {
    public:
        TaskNotify( Args && ... _args )
            : m_args( std::forward<Forwards &&>( _args ) ... )
        {

        }

        ~TaskNotify() override
        {
        }

    protected:
        bool _onRun() override
        {
            NotificationServiceInterface * service = NOTIFICATION_SERVICE();

            std::apply( &NotificationServiceInterface::notify<ID, Args ...>, std::tuple_cat( std::make_tuple( service ), m_args ) );

            return true;
        }

        void _onFinally() override
        {

        }

    protected:
        typedef Tuple<Args ...> TupleArgs;
        TupleArgs m_args;
    };
}