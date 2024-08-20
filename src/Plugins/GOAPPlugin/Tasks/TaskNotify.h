#pragma once

#include "GOAP/TaskInterface.h"

#include "Interface/NotificationServiceInterface.h"

#include "Kernel/Node.h"
#include "Kernel/Tuple.h"

namespace Mengine
{
    template<class ID, class ... Args>
    class TaskNotify
        : public GOAP::TaskInterface
    {
    public:
        TaskNotify( Args && ... _args )
            : m_args( std::forward<Args>( _args ) ... )
        {

        }

        ~TaskNotify() override
        {
        }

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override
        {
            GOAP_UNUSED( _node );

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