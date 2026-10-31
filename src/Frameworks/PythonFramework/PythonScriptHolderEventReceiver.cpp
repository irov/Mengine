#include "PythonScriptHolderEventReceiver.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonScriptHolderEventReceiver::PythonScriptHolderEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonScriptHolderEventReceiver::~PythonScriptHolderEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::object PythonScriptHolderEventReceiver::onScriptHolderKeepScript()
    {
        pybind::object script = m_cb.call();

        return script;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptHolderEventReceiver::onScriptHolderReleaseScript( const pybind::object & _script )
    {
        m_cb.call( _script );
    }
    //////////////////////////////////////////////////////////////////////////
}
