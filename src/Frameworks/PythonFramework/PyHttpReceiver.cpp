#include "PyHttpReceiver.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PyHttpReceiver::PyHttpReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PyHttpReceiver::~PyHttpReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PyHttpReceiver::initialize( const pybind::object & _cb, const pybind::args & _args )
    {
        m_cb = _cb;
        m_args = _args;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PyHttpReceiver::onHttpRequestComplete( const HttpResponseInterfacePtr & _response )
    {
        HttpRequestId requestId = _response->getRequestId();
        uint32_t status = 0; //ToDo remove
        const String & error = _response->getError();
        const String & data = _response->getData();
        EHttpCode code = _response->getCode();
        bool successful = _response->isSuccessful();

        m_cb.call_args( requestId, status, error, data, code, successful, m_args );
    }
    //////////////////////////////////////////////////////////////////////////
}
