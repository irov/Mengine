#pragma once

#include "Interface/ServantInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/String.h"
#include "Kernel/Data.h"
#include "Kernel/VectorString.h"
#include "Kernel/HttpCode.h"
#include "Kernel/HttpRequestId.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef VectorString HttpHeaders;
    //////////////////////////////////////////////////////////////////////////
    struct HttpRequestPostProperty
    {
        ConstString key;
        String value;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<HttpRequestPostProperty> HttpRequestPostProperties;
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class HttpRequestInterface> HttpRequestInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class HttpResponseInterface
        : public ServantInterface
    {
    public:
        virtual void setRequest( const HttpRequestInterfacePtr & _requestId ) = 0;
        virtual const HttpRequestInterfacePtr & getRequest() const = 0;

        virtual void appendHeaders( const Char * _ptr, size_t _size ) = 0;
        virtual const HttpHeaders & getHeaders() const = 0;

        virtual void appendData( const void * _ptr, size_t _size ) = 0;
        virtual const Data & getData() const = 0;

        virtual void setCode( EHttpCode _code ) = 0;
        virtual EHttpCode getCode() const = 0;

        virtual void setError( const String & _message, int32_t _code ) = 0;
        virtual const String & getErrorMessage() const = 0;
        virtual int32_t getErrorCode() const = 0;

        virtual void setSuccessful( bool _successful ) = 0;
        virtual bool isSuccessful() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<HttpResponseInterface> HttpResponseInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
