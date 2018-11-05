#include "Kernel/Scriptable.h"

#include "Interface/ScriptSystemInterface.h"

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
        (void)_kernel;

        if( m_scriptWrapper == nullptr )
        {
            return nullptr;
        }

        PyObject * embedding = m_scriptWrapper->wrap( this );

        return embedding;
    }
}
