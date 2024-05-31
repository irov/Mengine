#pragma once

#include "Interface/ServantInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/String.h"
#include "Kernel/Data.h"
#include "Kernel/VectorString.h"
#include "Kernel/HttpCode.h"

#include "Config/UniqueId.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef UniqueId HttpRequestId;
    //////////////////////////////////////////////////////////////////////////
    typedef VectorString HttpRequestHeaders;
    //////////////////////////////////////////////////////////////////////////
    struct HttpRequestPostProperty
    {
        ConstString key;
        String value;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<HttpRequestPostProperty> HttpRequestPostProperties;
    //////////////////////////////////////////////////////////////////////////
    class HttpResponseInterface
        : public ServantInterface
    {
    public:
        virtual void setRequestId( HttpRequestId _requestId ) = 0;
        virtual HttpRequestId getRequestId() const = 0;

        virtual void appendHeaders( const Char * _ptr, size_t _size ) = 0;
        virtual const HttpRequestHeaders & getHeaders() const = 0;

        virtual void appendData( const Char * _ptr, size_t _size ) = 0;
        virtual const String & getData() const = 0;

        virtual void setCode( EHttpCode _code ) = 0;
        virtual EHttpCode getCode() const = 0;

        virtual void setError( const String & _error, int32_t _code ) = 0;
        virtual const String & getErrorMessage() const = 0;
        virtual int32_t getErrorCode() const = 0;

        virtual void setSuccessful( bool _successful ) = 0;
        virtual bool isSuccessful() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<HttpResponseInterface> HttpResponseInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
