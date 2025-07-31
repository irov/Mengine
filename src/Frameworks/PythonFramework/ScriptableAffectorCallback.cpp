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

        PythonCallbackProvider::initialize( _cb, _args );
    }
    //////////////////////////////////////////////////////////////////////////
    void ScriptableAffectorCallback::onAffectorEnd( bool _isEnd )
    {
        ScriptablePtr scriptable = m_scriptable;
        m_scriptable = nullptr;

        this->call_cb( scriptable, _isEnd );
    }
    //////////////////////////////////////////////////////////////////////////
}