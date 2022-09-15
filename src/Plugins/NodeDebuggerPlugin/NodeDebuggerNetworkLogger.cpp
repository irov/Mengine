#include "NodeDebuggerNetworkLogger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    NodeDebuggerNetworkLogger::NodeDebuggerNetworkLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    NodeDebuggerNetworkLogger::~NodeDebuggerNetworkLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerNetworkLogger::request( HttpRequestID _id, const String & _url )
    {
        RequestData requestData;

        requestData.id = _id;
        requestData.url = _url;
        requestData.type = STRINGIZE_STRING_LOCAL( "Request" );

        m_requestDatas.push_back( requestData );

        m_sceneDataProvider->setUpdateSceneFlag( true );

        this->clearDatas_();
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerNetworkLogger::response( HttpRequestID _id, const String & _url )
    {
        RequestData responseData;

        responseData.id = _id;
        responseData.url = _url;
        responseData.type = STRINGIZE_STRING_LOCAL( "Response" );

        m_requestDatas.push_back( responseData );

        m_sceneDataProvider->setUpdateSceneFlag( true );

        this->clearDatas_();
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerNetworkLogger::setSceneDataProvider( const SceneDataProviderInterfacePtr & _sceneDataProvider )
    {
        m_sceneDataProvider = _sceneDataProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerNetworkLogger::foreachData( const LambdaNetworkData & _lambda )
    {
        for( const RequestData & data : m_requestDatas )
        {
            _lambda( data );
        };
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerNetworkLogger::clearDatas_()
    {
        if( m_requestDatas.size() <= 128 )
        {
            return;
        }

        m_requestDatas.pop_front();
    }
    //////////////////////////////////////////////////////////////////////////
}