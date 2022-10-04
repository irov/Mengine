#pragma once

#include "Plugins/cURLPlugin/cURLInterface.h"

#include "SceneDataProviderInterface.h"

#include "Kernel/List.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct RequestData
    {
        ConstString type;
        HttpRequestID id;
        String url;
    };
    //////////////////////////////////////////////////////////////////////////
    class NodeDebuggerNetworkLogger
        : public Factorable
        , public cURLRequestListenerInterface
    {
        DECLARE_FACTORABLE( NodeDebuggerNetworkLogger );

    public:
        NodeDebuggerNetworkLogger();
        ~NodeDebuggerNetworkLogger() override;

    public:
        void setSceneDataProvider( const SceneDataProviderInterfacePtr & _sceneDataProvider );

    public:
        typedef Lambda<void( const RequestData & data )> LambdaNetworkData;
        void foreachData( const LambdaNetworkData & _lambda );

    protected:
        void request( HttpRequestID _id, const String & _url ) override;
        void response( HttpRequestID _id, const String & _url ) override;

    protected:
        void clearDatas_();

    protected:
        SceneDataProviderInterfacePtr m_sceneDataProvider;

        typedef List<RequestData> ListRequestDatas;
        ListRequestDatas m_requestDatas;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<NodeDebuggerNetworkLogger> NodeDebuggerNetworkLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}