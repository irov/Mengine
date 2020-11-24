#pragma once

#include "cURLInterface.h"

#include "Kernel/List.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct RequestData
    {
        String type;
        HttpRequestID id;
        String url;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef List<RequestData> ListRequestData;
    //////////////////////////////////////////////////////////////////////////
    class cURLRequestListener
        : public Factorable
        , public cURLRequestListenerInterface
    {
    public:
        cURLRequestListener();
        ~cURLRequestListener() override;

    public:
        void request( HttpRequestID  _id, const String & _url ) override;
        void response( HttpRequestID  _id, const String & _url ) override;

    public:
        const ListRequestData & getRequestData() const;

    protected:
        ListRequestData m_data;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLRequestListener> cURLRequestListenerPtr;
    //////////////////////////////////////////////////////////////////////////
}