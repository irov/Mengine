#include "Kernel/Scriptable.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Interface/ScriptServiceInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Scriptable::Scriptable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Scriptable::~Scriptable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Scriptable::setScriptWrapper( const ScriptWrapperInterfacePtr & _scriptWrapper )
    {
        m_scriptWrapper = _scriptWrapper;
    }
    //////////////////////////////////////////////////////////////////////////
    const ScriptWrapperInterfacePtr & Scriptable::getScriptWrapper() const
    {
        return m_scriptWrapper;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * Scriptable::_embedded( pybind::kernel_interface * _kernel )
    {
        MENGINE_UNUSED( _kernel );

        MENGINE_ASSERTION_MEMORY_PANIC( m_scriptWrapper, nullptr );

        PyObject * embedding = m_scriptWrapper->wrap( this );

        return embedding;
    }
}
