#   pragma once

#   include "GOAP/GOAP.h"

#   include "pybind/bindable.hpp"

#   include "pybind/object.hpp"
#   include "pybind/tuple.hpp"

namespace Menge
{
    class PybindSource
        : public GOAP::Source
        , public pybind::bindable
    {
    public:
        PybindSource();
        ~PybindSource();

    protected:
        void addFunction( const pybind::object & _obj, const pybind::detail::args_operator_t & _args );
        void addCallback( const pybind::object & _obj, const pybind::detail::args_operator_t & _args );
        void addScope( const pybind::object & _obj, const pybind::detail::args_operator_t & _args );
        pybind::tuple addIf( const pybind::object & _obj, const pybind::detail::args_operator_t & _args );
        pybind::tuple addSwitch( size_t _count, const pybind::object & _obj, const pybind::detail::args_operator_t & _args );
        GOAP::SourcePtr addRepeat( const pybind::object & _obj, const pybind::detail::args_operator_t & _args );
        GOAP::SourcePtr addGuard( const pybind::object & _begin, const pybind::object & _end, const pybind::detail::args_operator_t & _args );

    protected:
        GOAP::SourcePtr _provideSource() override;

    protected:
        PyObject * _embedded() override;

    public:
        pybind::kernel_interface * m_kernel;
    };

    typedef GOAP::IntrusivePtr<PybindSource> PybindSourcePtr;
}