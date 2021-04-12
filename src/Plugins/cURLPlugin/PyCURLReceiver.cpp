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
    void PyCURLReceiver::onHttpRequestComplete( HttpRequestID _id, uint32_t _status, const String & _error, const cURLHeaders & _headers, const String & _response, uint32_t _code, bool _successful )
    {
        MENGINE_UNUSED( _headers );

        m_cb.call_args( _id, _status, _error, _response, _code, _successful, m_args );
    }
    //////////////////////////////////////////////////////////////////////////
}