#include "TaskRouterButtonClick.h"

#include "ButtonEventReceiverInterface.h"

#include "Kernel/FactorableUnique.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        typedef Lambda<void()> LambdaTaskRouterButtonClickEvent;
        //////////////////////////////////////////////////////////////////////////
        class TaskRouterButtonClickEventReceiver
            : public ButtonEventReceiverInterface
            , public Factorable
        {
        public:
            explicit TaskRouterButtonClickEventReceiver( const LambdaTaskRouterButtonClickEvent & _event )
                : m_event( _event )
            {
            }

            ~TaskRouterButtonClickEventReceiver() override
            {
            }

        protected:
            void onButtonMouseEnter() override
            {
                //Empty
            }

            void onButtonMouseLeave() override
            {
                //Empty
            }

            void onButtonMousePush() override
            {
                //Empty
            }

            void onButtonMousePressed() override
            {
                //Empty
            }

            void onButtonMouseRelease() override
            {
                //Empty
            }

            void onButtonMouseButton() override
            {
                m_event();
            }

            void onButtonMouseButtonBegin() override
            {
                //Empty
            }

            void onButtonMouseButtonEnd() override
            {
                //Empty
            }

        protected:
            LambdaTaskRouterButtonClickEvent m_event;

        };
    }
    //////////////////////////////////////////////////////////////////////////
    TaskRouterButtonClick::TaskRouterButtonClick( GOAP::Allocator * _allocator, const ConstString & _state, const RouterButtonEventReceiverPtr & _router )
        : GOAP::TaskInterface( _allocator )
        , m_state( _state )
        , m_router( _router )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskRouterButtonClick::~TaskRouterButtonClick()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskRouterButtonClick::_onRun( GOAP::NodeInterface * _node )
    {
        auto lambda = [_node]()
        {
            _node->complete();
        };

        EventReceiverInterfacePtr receiver = Helper::makeFactorableUnique<Detail::TaskRouterButtonClickEventReceiver>( MENGINE_DOCUMENT_FUNCTION, lambda );

        m_router->addButtonEventReceiver( m_state, receiver );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskRouterButtonClick::_onFinally()
    {
        m_router->removeButtonEventReceiver( m_state );

        m_router = nullptr;

        m_state = ConstString::none();
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskRouterButtonClick::_onSkipable() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}