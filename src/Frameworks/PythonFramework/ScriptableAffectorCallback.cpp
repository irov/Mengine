#include "ScriptableAffectorCallback.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ScriptableAffectorCallback::ScriptableAffectorCallback()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ScriptableAffectorCallback::~ScriptableAffectorCallback()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ScriptableAffectorCallback::initialize( const ScriptablePtr & _scriptable, const pybind::object & _cb, const pybind::args & _args )
    {
        m_scriptable = _scriptable;
        m_cb = _cb;
        m_args = _args;
    }
    //////////////////////////////////////////////////////////////////////////
    void ScriptableAffectorCallback::onAffectorEnd( uint32_t _id, bool _isEnd )
    {
        ScriptablePtr scriptable = m_scriptable;
        m_scriptable = nullptr;

        if( m_cb.is_invalid() == true )
        {
            return;
        }

        if( m_cb.is_none() == true )
        {
            return;
        }

        m_cb.call_args( scriptable, _id, _isEnd, m_args );
    }
    //////////////////////////////////////////////////////////////////////////
}