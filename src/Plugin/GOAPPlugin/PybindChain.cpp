#   include "PybindChain.h"

#   include "pybind/kernel.hpp"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    PybindChain::PybindChain( const GOAP::SourcePtr & _source )
        : GOAP::Chain(_source)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * PybindChain::_embedded()
    {
        pybind::kernel_interface * kernel = pybind::get_kernel();

        PyObject * py_obj = kernel->scope_create_holder_t( this );

        return py_obj;
    }
}