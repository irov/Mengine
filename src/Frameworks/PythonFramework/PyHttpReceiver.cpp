#include "PyHttpReceiver.h"

#include "Kernel/DataHelper.h"

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
    void PyHttpReceiver::onHttpRequestComplete( const HttpResponseInterfacePtr & _response )
    {
        const HttpRequestInterfacePtr & request = _response->getRequest();

        HttpRequestId requestId = request->getRequestId();
        uint32_t status = 0; //ToDo remove
        const String & errorMessage = _response->getErrorMessage();
        
        //ToDo
        int32_t errorCode = _response->getErrorCode();
        MENGINE_UNUSED( errorCode );

        const String & json = _response->getJSON();
        EHttpCode code = _response->getCode();
        bool successful = _response->isSuccessful();
                
        this->call_cb( requestId, status, errorMessage, json, code, successful );
    }
    //////////////////////////////////////////////////////////////////////////
}
