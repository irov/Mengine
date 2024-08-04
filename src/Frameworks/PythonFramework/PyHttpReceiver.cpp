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
        const HttpRequestInterfacePtr & request = _response->getRequest();

        HttpRequestId requestId = request->getRequestId();
        uint32_t status = 0; //ToDo remove
        const String & errorMessage = _response->getErrorMessage();
        
        //ToDo
        int32_t errorCode = _response->getErrorCode();
        MENGINE_UNUSED( errorCode );

        const String & json = _response->getJson();
        EHttpCode code = _response->getCode();
        bool successful = _response->isSuccessful();

        m_cb.call_args( requestId, status, errorMessage, json, code, successful, m_args );
    }
    //////////////////////////////////////////////////////////////////////////
}
