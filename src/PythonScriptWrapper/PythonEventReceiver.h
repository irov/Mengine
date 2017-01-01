#   pragma once

#   include <pybind/object.hpp>

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    class PythonEventReceiver
    {
    public:
        void initialize( const pybind::object & _cb )
        {
            m_cb = _cb;
        }

    protected:
        pybind::object m_cb;
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T_Receiver>
        void registerEventReceiver( const pybind::dict & _kwds, Eventable * _eventable, const char * _method, uint32_t _event )
        {
            if( _kwds.exist( _method ) == false )
            {
                return;
            }

            pybind::object py_event = _kwds[_method];

            if( py_event.is_none() == true )
            {
                _eventable->removeEventReceiver( _event );
            }
            else
            {
                static FactoryPoolStore<T_Receiver, 32> factory;

                T_Receiver * receiver = factory.createObject();

                receiver->initialize( py_event );

                _eventable->registerEventReceiver( _event, receiver );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T_Receiver>
        void registerEventReceiverMethod( const pybind::object & _obj, Eventable * _eventable, const char * _method, uint32_t _event )
        {
            if( _obj.has_attr( _method ) == false )
            {
                return;
            }
                
            pybind::object py_method = _obj.get_attr( _method );

            if( py_method.is_none() == true )
            {
                _eventable->removeEventReceiver( _event );
            }
            else
            {
                static FactoryPoolStore<T_Receiver, 32> factory;

                T_Receiver * receiver = factory.createObject();

                receiver->initialize( py_method );

                _eventable->registerEventReceiver( _event, receiver );
            }            
        }
    }
}