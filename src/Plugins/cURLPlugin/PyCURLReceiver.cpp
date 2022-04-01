#include "PyCURLReceiver.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PyCURLReceiver::PyCURLReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PyCURLReceiver::~PyCURLReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PyCURLReceiver::initialize( const pybind::object & _cb, const pybind::args & _args )
    {
        m_cb = _cb;
        m_args = _args;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PyCURLReceiver::onHttpRequestComplete( const cURLResponseData & _response )
    {
        m_cb.call_args( _response.id, _response.status, _response.error, _response.data, _response.code, _response.successful, m_args );
    }
    //////////////////////////////////////////////////////////////////////////
}