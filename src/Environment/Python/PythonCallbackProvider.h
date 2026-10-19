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
        pybind::detail::extract_operator_t call_cb( Args && ... _args ) const
        {
            if( m_cb.is_invalid() == true )
            {
                return this->make_invalid_();
            }

            if( m_cb.is_callable() == false )
            {
                return this->make_invalid_();
            }

            return m_cb.call_args( std::forward<Args>( _args ) ..., m_args );
        }

        template<class ... Args>
        pybind::detail::extract_operator_t call_method( const Char * _method, Args && ... _args ) const
        {
            if( m_cb.is_invalid() == true )
            {
                return this->make_invalid_();
            }

            if( m_cb.is_dict() == true )
            {
                pybind::object py_cb = pybind::dict( m_cb )[_method];

                if( py_cb.is_callable() == false )
                {
                    return this->make_invalid_();
                }

                return py_cb.call_args( std::forward<Args>( _args ) ..., m_args );
            }
            else if( m_cb.is_callable() == true )
            {
                return m_cb.call_args( _method, std::forward<Args>( _args ) ..., m_args );
            }

            return this->make_invalid_();
        }

    protected:
        pybind::detail::extract_operator_t make_invalid_() const
        {
            // A result of the kernel, which a typed extract reports as invalid
            // rather than reaches through a null kernel.
            pybind::kernel_interface * kernel = pybind::get_kernel();

            return pybind::detail::extract_operator_t( kernel, nullptr, pybind::borrowed );
        }

    private:
        pybind::object m_cb;
        pybind::args m_args;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonCallbackProvider> PythonCallbackProviderPtr;
    //////////////////////////////////////////////////////////////////////////
}
