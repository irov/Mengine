#include "Scriptable.h"

#include "Kernel/ExceptionHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Scriptable::Scriptable()
        : m_embed( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Scriptable::~Scriptable()
    {
        MENGINE_ASSERTION_FATAL( m_embed == nullptr, "scriptable distruction embed is not NULL" );
    }
    //////////////////////////////////////////////////////////////////////////
    void Scriptable::setEmbed( pybind::kernel_interface * _kernel, PyObject * _embed )
    {
        MENGINE_UNUSED( _kernel );

        m_embed = _embed;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * Scriptable::getEmbed( pybind::kernel_interface * _kernel )
    {
        MENGINE_UNUSED( _kernel );

        m_embed = m_scriptWrapper->wrap( this, m_embed );

        return m_embed;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Scriptable::isEmbed() const
    {
        return m_embed != nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void Scriptable::clearEmbed()
    {
        m_embed = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void Scriptable::unwrap()
    {
        if( m_embed != nullptr )
        {
            PyObject * embed = m_embed;
            m_embed = nullptr;
            m_scriptWrapper->unwrap( embed );
        }
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
}
