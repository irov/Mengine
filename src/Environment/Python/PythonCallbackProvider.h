#pragma once

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/Mixin.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonCallbackProvider
        : public Mixin
    {
    public:
        PythonCallbackProvider();
        PythonCallbackProvider( const pybind::object & _cb, const pybind::args & _args );
        ~PythonCallbackProvider() override;

    public:
        void initialize( const pybind::object & _cb, const pybind::args & _args );
        void finalize();

    public:
        const pybind::object & get_cb() const;
        const pybind::args & get_args() const;

    public:
        template<class ... Args>
        pybind::detail::extract_operator_t call_cb( Args && ... _args )
        {
            if( m_cb.is_invalid() == true )
            {
                return pybind::detail::extract_operator_t();
            }

            if( m_cb.is_callable() == false )
            {
                return pybind::detail::extract_operator_t();
            }

            return m_cb.call_args( std::forward<Args>( _args ) ..., m_args );
        }

        template<class ... Args>
        pybind::detail::extract_operator_t call_method( const Char * _method, Args && ... _args )
        {
            if( m_cb.is_invalid() == true )
            {
                return pybind::detail::extract_operator_t();
            }

            if( m_cb.is_dict() == true )
            {
                pybind::object py_cb = pybind::dict( m_cb )[_method];

                if( py_cb.is_callable() == false )
                {
                    return pybind::detail::extract_operator_t();
                }

                return py_cb.call_args( std::forward<Args>( _args ) ..., m_args );
            }
            else if( m_cb.is_callable() == true )
            {
                return m_cb.call_args( _method, std::forward<Args>( _args ) ..., m_args );
            }

            return pybind::detail::extract_operator_t();
        }

    private:
        pybind::object m_cb;
        pybind::args m_args;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonCallbackProvider> PythonCallbackProviderPtr;
    //////////////////////////////////////////////////////////////////////////
}
