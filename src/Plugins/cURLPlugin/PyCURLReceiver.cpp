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
    void PyCURLReceiver::onHttpRequestComplete( const cURLResponseInterfacePtr & _response )
    {
        HttpRequestID requestId = _response->getRequestId();
        uint32_t status = _response->getStatus();
        const String & error = _response->getError();
        const String & data = _response->getData();
        uint32_t code = _response->getCode();
        bool successful = _response->isSuccessful();

        m_cb.call_args( requestId, status, error, data, code, successful, m_args );
    }
    //////////////////////////////////////////////////////////////////////////
}