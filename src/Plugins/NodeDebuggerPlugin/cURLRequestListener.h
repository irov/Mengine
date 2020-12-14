#pragma once

#include "Plugins/cURLPlugin/cURLInterface.h"

#include "SceneDataProviderInterface.h"

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
    typedef Vector<RequestData> VectorRequestData;
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

        void setSceneDataProvider( const SceneDataProviderInterfacePtr & _sceneDataProvider );

    public:
        void getPreparedData( VectorRequestData * _outData );

    protected:
        VectorRequestData m_data;
        SceneDataProviderInterfacePtr m_sceneDataProvider;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLRequestListener> cURLRequestListenerPtr;
    //////////////////////////////////////////////////////////////////////////
}