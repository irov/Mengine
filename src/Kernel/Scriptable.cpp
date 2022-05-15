#include "Scriptable.h"

#include "Kernel/ExceptionHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Environment/Python/PythonIncluder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Scriptable::Scriptable()
        : m_kernel( nullptr )
        , m_embed( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Scriptable::~Scriptable()
    {
        MENGINE_ASSERTION_FATAL( m_embed == nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    void Scriptable::setEmbed( pybind::kernel_interface * _kernel, PyObject * _embed )
    {
        MENGINE_ASSERTION_FATAL( m_embed == nullptr );

#if defined(MENGINE_DEBUG) && defined(MENGINE_USE_SCRIPT_SERVICE)
        if( _kernel->is_object_bindable( _embed ) == false )
        {
            MENGINE_THROW_EXCEPTION( "'%s' but scope not setup bindable"
                , _kernel->object_repr_type( _embed ).c_str()
            );
        }
#endif

        m_kernel = _kernel;
        m_embed = _embed;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * Scriptable::getEmbed( pybind::kernel_interface * _kernel )
    {
        MENGINE_UNUSED( _kernel );

#ifdef MENGINE_USE_SCRIPT_SERVICE
        if( m_embed == nullptr )
        {
            PyObject * embed = this->_embedded( _kernel );

            this->setEmbed( _kernel, embed );
        }
        else
        {
            _kernel->incref( m_embed );
        }
#endif

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
        m_kernel = nullptr;
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
    PyObject * Scriptable::_embedded( pybind::kernel_interface * _kernel )
    {
        MENGINE_UNUSED( _kernel );

        MENGINE_ASSERTION_MEMORY_PANIC( m_scriptWrapper );

        PyObject * embedding = m_scriptWrapper->wrap( this );

        return embedding;
    }
    //////////////////////////////////////////////////////////////////////////
}
