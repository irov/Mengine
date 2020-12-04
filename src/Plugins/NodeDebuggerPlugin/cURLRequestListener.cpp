#include "cURLRequestListener.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    cURLRequestListener::cURLRequestListener()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    cURLRequestListener::~cURLRequestListener()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLRequestListener::request( HttpRequestID _id, const String & _url )
    {
        RequestData requestData;

        requestData.id = _id;
        requestData.url = _url;
        requestData.type = "Request";

        m_data.push_back( requestData );

        m_sceneDataProvider->setUpdateSceneFlag( true );
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLRequestListener::response( HttpRequestID _id, const String & _url )
    {
        RequestData responseData;

        responseData.id = _id;
        responseData.url = _url;
        responseData.type = "Response";

        m_data.push_back( responseData );

        m_sceneDataProvider->setUpdateSceneFlag( true );
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLRequestListener::setSceneDataProvider( const SceneDataProviderInterfacePtr & _sceneDataProvider )
    {
        m_sceneDataProvider = _sceneDataProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLRequestListener::getPreparedData( VectorRequestData * _outData )
    {
        std::copy( m_data.begin(), m_data.end(), std::back_inserter( *_outData ) );
    }
    //////////////////////////////////////////////////////////////////////////
}