#pragma once

#include "Kernel/Eventable.h"

#include "Kernel/FactorableUnique.h"

#include "Config/Char.h"

#include "pybind/object.hpp"
#include "pybind/dict.hpp"
#include "pybind/module.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonEventReceiver
        : public Mixin
    {
    public:
        PythonEventReceiver()
            : m_kernel( nullptr )
        {
        };

        ~PythonEventReceiver() override
        {
        };

    public:
        void initialize( pybind::kernel_interface * _kernel, const pybind::object & _cb )
        {
            m_kernel = _kernel;
            m_cb = _cb;
        }

    protected:
        pybind::kernel_interface * m_kernel;
        pybind::object m_cb;
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T_Receiver>
        void registerPythonEventReceiver( pybind::kernel_interface * _kernel, const pybind::dict & _kwds, Eventable * _eventable, const Char * _method, uint32_t _event )
        {
            EventationInterface * event = _eventable->getEventation();

            if( event == nullptr )
            {
                return;
            }

            if( _kwds.exist( _method ) == false )
            {
                return;
            }

            pybind::object py_event = _kwds[_method];

            if( py_event.is_none() == true )
            {
                event->removeEventReceiver( _event );
            }
            else
            {
                IntrusivePtr<T_Receiver> receiver = Helper::makeFactorableUnique<T_Receiver>();

                receiver->initialize( _kernel, py_event );

                event->registerEventReceiver( _event, receiver );
            }

#ifdef MENGINE_DEBUG
            _kwds.remove( _method );
#endif
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T_Receiver>
        void registerPythonEventReceiverModule( pybind::kernel_interface * _kernel, const pybind::module & _module, Eventable * _eventable, const Char * _method, uint32_t _event )
        {
            EventationInterface * event = _eventable->getEventation();

            if( event == nullptr )
            {
                return;
            }

            if( _module.has_attr( _method ) == false )
            {
                return;
            }

            pybind::object py_method = _module.get_attr( _method );

            if( py_method.is_none() == true )
            {
                event->removeEventReceiver( _event );
            }
            else
            {
                IntrusivePtr<T_Receiver> receiver = Helper::makeFactorableUnique<T_Receiver>();

                receiver->initialize( _kernel, py_method );

                event->registerEventReceiver( _event, receiver );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T_Receiver>
        void registerPythonEventReceiverMethod( pybind::kernel_interface * _kernel, const pybind::object & _obj, Eventable * _eventable, const Char * _method, uint32_t _event )
        {
            EventationInterface * event = _eventable->getEventation();

            if( event == nullptr )
            {
                return;
            }

            if( _obj.has_attr( _method ) == false )
            {
                return;
            }

            pybind::object py_method = _obj.get_attr( _method );

            if( py_method.is_none() == true )
            {
                event->removeEventReceiver( _event );
            }
            else
            {
                IntrusivePtr<T_Receiver> receiver = Helper::makeFactorableUnique<T_Receiver>();

                receiver->initialize( _kernel, py_method );

                event->registerEventReceiver( _event, receiver );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T_Receiver>
        void registerPythonEventReceiverMethod( pybind::kernel_interface * _kernel, const pybind::object & _obj, const EventablePtr & _eventable, const Char * _method, uint32_t _event )
        {
            registerPythonEventReceiverMethod<T_Receiver>( _kernel, _obj, _eventable.get(), _method, _event );
        }
    }
}
