#include "RouterButtonEventReceiver.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    RouterButtonEventReceiver::RouterButtonEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RouterButtonEventReceiver::~RouterButtonEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void RouterButtonEventReceiver::addButtonEventReceiver( const ConstString & _state, const ButtonEventReceiverInterfacePtr & _button )
    {
        ButtonEventReceiverInterfacePtr delreceiver = m_receivers.change( _state, _button );

        MENGINE_UNUSED( delreceiver );

        MENGINE_ASSERTION_FATAL( delreceiver == nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    ButtonEventReceiverInterfacePtr RouterButtonEventReceiver::removeButtonEventReceiver( const ConstString & _state )
    {
        ButtonEventReceiverInterfacePtr delreceiver = m_receivers.erase( _state );

        MENGINE_ASSERTION_FATAL( delreceiver != nullptr );

        return delreceiver;
    }
    //////////////////////////////////////////////////////////////////////////
    void RouterButtonEventReceiver::setState( const ConstString & _state )
    {
        m_state = _state;
    }
    //////////////////////////////////////////////////////////////////////////
    void RouterButtonEventReceiver::onButtonMouseEnter()
    {
        const ButtonEventReceiverInterfacePtr & receiver = m_receivers.find( m_state );

        MENGINE_ASSERTION_FATAL( receiver != nullptr );

        receiver->onButtonMouseEnter();
    }
    //////////////////////////////////////////////////////////////////////////
    void RouterButtonEventReceiver::onButtonMouseLeave()
    {
        const ButtonEventReceiverInterfacePtr & receiver = m_receivers.find( m_state );

        MENGINE_ASSERTION_FATAL( receiver != nullptr );

        receiver->onButtonMouseLeave();
    }
    //////////////////////////////////////////////////////////////////////////
    void RouterButtonEventReceiver::onButtonMousePush()
    {
        const ButtonEventReceiverInterfacePtr & receiver = m_receivers.find( m_state );

        MENGINE_ASSERTION_FATAL( receiver != nullptr );

        receiver->onButtonMousePush();
    }
    //////////////////////////////////////////////////////////////////////////
    void RouterButtonEventReceiver::onButtonMousePressed()
    {
        const ButtonEventReceiverInterfacePtr & receiver = m_receivers.find( m_state );

        MENGINE_ASSERTION_FATAL( receiver != nullptr );

        receiver->onButtonMousePressed();
    }
    //////////////////////////////////////////////////////////////////////////
    void RouterButtonEventReceiver::onButtonMouseRelease()
    {
        const ButtonEventReceiverInterfacePtr & receiver = m_receivers.find( m_state );

        MENGINE_ASSERTION_FATAL( receiver != nullptr );

        receiver->onButtonMouseRelease();
    }
    //////////////////////////////////////////////////////////////////////////
    void RouterButtonEventReceiver::onButtonMouseButton()
    {
        const ButtonEventReceiverInterfacePtr & receiver = m_receivers.find( m_state );

        MENGINE_ASSERTION_FATAL( receiver != nullptr );

        receiver->onButtonMouseButton();
    }
    //////////////////////////////////////////////////////////////////////////
    void RouterButtonEventReceiver::onButtonMouseButtonBegin()
    {
        const ButtonEventReceiverInterfacePtr & receiver = m_receivers.find( m_state );

        MENGINE_ASSERTION_FATAL( receiver != nullptr );

        receiver->onButtonMouseButtonBegin();
    }
    //////////////////////////////////////////////////////////////////////////
    void RouterButtonEventReceiver::onButtonMouseButtonEnd()
    {
        const ButtonEventReceiverInterfacePtr & receiver = m_receivers.find( m_state );

        MENGINE_ASSERTION_FATAL( receiver != nullptr );

        receiver->onButtonMouseButtonEnd();
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        RouterButtonEventReceiverPtr makeRouterButtonEventReceiver( const ButtonPtr & _button, const DocumentPtr & _doc )
        {
            RouterButtonEventReceiverPtr router = Helper::makeFactorableUnique<RouterButtonEventReceiver>( _doc );

            EventationInterface * eventation = _button->getEventation();

            EventReceiverInterfacePtr oldreceiver = eventation->addEventReceiver( EVENT_BUTTON_MOUSE_BUTTON, router );

            MENGINE_ASSERTION_FATAL_RETURN( oldreceiver == nullptr, nullptr, "event EVENT_BUTTON_MOUSE_BUTTON override" );

            return router;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}