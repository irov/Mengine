#pragma once

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    template<class T>
    class PythonCallbackProvider
        : public T
        , public Factorable
    {
    public:
        PythonCallbackProvider( pybind::kernel_interface * _kernel, const pybind::dict & _cbs, const pybind::args & _args )
            : m_cbs( _cbs )
            , m_args( _args )
        {
        }

        ~PythonCallbackProvider()
        {
        }

    protected:
        template<class ... Args>
        void call_cbs( const Char * _method, Args && ... _args )
        {
            pybind::object py_cb = m_cbs[_method];

            if( py_cb.is_callable() == false )
            {
                return;
            }

            py_cb.call_args( std::forward<Args>( _args ) ..., m_args );
        }

    protected:
        template<class T, class K>
        pybind::dict make_dict( const Map<T, K> & _params ) const
        {
            pybind::dict d( m_kernel );

            for( auto && [key, value] : _params )
            {
                d[key] = value;
            }

            return d;
        }

    protected:
        pybind::dict m_cbs;
        pybind::args m_args;
    }
}