#include "PythonCallbackProvider.h"

#include "Environment/Python/PythonTraceback.h"

#include "Kernel/Assertion.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonCallbackProvider::PythonCallbackProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonCallbackProvider::PythonCallbackProvider( const pybind::object & _cb, const pybind::args & _args )
        : m_cb( _cb )
        , m_args( _args )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonCallbackProvider::~PythonCallbackProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonCallbackProvider::initialize( const pybind::object & _cb, const pybind::args & _args )
    {
        MENGINE_ASSERTION_FATAL( _cb.is_dict() == true || _cb.is_callable() == true, "callback must be callable or dict, but is '%s' type '%s'\ntraceback:\n%s"
            , _cb.repr().c_str()
            , _cb.repr_type().c_str()
            , Helper::getPythonStatetrace()
        );

        m_cb = _cb;
        m_args = _args;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonCallbackProvider::finalize()
    {
        m_cb = nullptr;
        m_args = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    const pybind::object & PythonCallbackProvider::get_cb() const
    {
        return m_cb;
    }
    //////////////////////////////////////////////////////////////////////////
    const pybind::args & PythonCallbackProvider::get_args() const
    {
        return m_args;
    }
    //////////////////////////////////////////////////////////////////////////
}
