#include "Scriptable.h"

#include "Kernel/Exception.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ScriptWrapperInterface.h"

#include "pybind/kernel.hpp"

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
    }
    //////////////////////////////////////////////////////////////////////////
    void Scriptable::setEmbed( pybind::kernel_interface * _kernel, PyObject * _embed )
    {
#ifdef MENGINE_DEBUG
        if( _kernel->is_object_bindable( _embed ) == false )
        {
            MENGINE_THROW_EXCEPTION( "'%s' but scope not settup bindable"
                , _kernel->object_repr_type( _embed )
            );

            return;
        }
#endif
        m_kernel = _kernel;
        m_embed = _embed;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * Scriptable::getEmbed( pybind::kernel_interface * _kernel )
    {
        if( m_embed == nullptr )
        {
            PyObject * embed = this->_embedded( _kernel );

            this->setEmbed( _kernel, embed );
        }
        else
        {
            _kernel->incref( m_embed );
        }

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
        if( m_kernel != nullptr )
        {
            PyObject * embed = m_embed;
            m_embed = nullptr;
            m_kernel->unwrap( embed );
            m_kernel = nullptr;
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

        MENGINE_ASSERTION_MEMORY_PANIC( m_scriptWrapper, nullptr );

        PyObject * embedding = m_scriptWrapper->wrap( this );

        return embedding;
    }
}
