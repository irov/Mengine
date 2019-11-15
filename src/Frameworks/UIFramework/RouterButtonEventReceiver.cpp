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
        m_receivers.emplace( _state, _button );
    }
    //////////////////////////////////////////////////////////////////////////
    ButtonEventReceiverInterfacePtr RouterButtonEventReceiver::removeButtonEventReceiver( const ConstString & _state )
    {
        ButtonEventReceiverInterfacePtr delreceiver = m_receivers.erase( _state );

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
}