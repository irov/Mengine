#include "AndroidHttpSystem.h"

#include "AndroidHttpRequestPing.h"
#include "AndroidHttpRequestGetMessage.h"
#include "AndroidHttpRequestPostMessage.h"
#include "AndroidHttpRequestHeaderData.h"
#include "AndroidHttpRequestGetAsset.h"
#include "AndroidHttpRequestDeleteMessage.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( HttpSystem, Mengine::AndroidHttpSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidHttpSystem::AndroidHttpSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidHttpSystem::~AndroidHttpSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidHttpSystem::_initializeService()
    {
        m_factoryRequestPing = Helper::makeFactoryPool<AndroidHttpRequestPing, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRequestGetMessage = Helper::makeFactoryPool<AndroidHttpRequestGetMessage, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRequestPostMessage = Helper::makeFactoryPool<AndroidHttpRequestPostMessage, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRequestDeleteMessage = Helper::makeFactoryPool<AndroidHttpRequestDeleteMessage, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRequestHeaderData = Helper::makeFactoryPool<AndroidHttpRequestHeaderData, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRequestGetAsset = Helper::makeFactoryPool<AndroidHttpRequestGetAsset, 16>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidHttpSystem::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRequestPing );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRequestGetAsset );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRequestPostMessage );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRequestDeleteMessage );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRequestHeaderData );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRequestGetMessage );

        m_factoryRequestPing = nullptr;
        m_factoryRequestGetAsset = nullptr;
        m_factoryRequestPostMessage = nullptr;
        m_factoryRequestDeleteMessage = nullptr;
        m_factoryRequestHeaderData = nullptr;
        m_factoryRequestGetMessage = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestInterfacePtr AndroidHttpSystem::createHttpRequestPing( const DocumentInterfacePtr & _doc )
    {
        AndroidHttpRequestPingPtr request = m_factoryRequestPing->createObject( _doc );

        return request;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestInterfacePtr AndroidHttpSystem::createHttpRequestGetMessage( const DocumentInterfacePtr & _doc )
    {
        AndroidHttpRequestGetMessagePtr request = m_factoryRequestGetMessage->createObject( _doc );

        return request;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestInterfacePtr AndroidHttpSystem::createHttpRequestPostMessage( const HttpRequestPostProperties & _params, const DocumentInterfacePtr & _doc )
    {
        AndroidHttpRequestPostMessagePtr request = m_factoryRequestPostMessage->createObject( _doc );

        request->setPostProperties( _params );

        return request;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestInterfacePtr AndroidHttpSystem::createHttpRequestHeaderData( const Data & _data, const DocumentInterfacePtr & _doc )
    {
        AndroidHttpRequestHeaderDataPtr request = m_factoryRequestHeaderData->createObject( _doc );

        request->setData( _data );

        return request;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestInterfacePtr AndroidHttpSystem::createHttpRequestGetAsset( const String & _login, const String & _password, const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc )
    {
        AndroidHttpRequestGetAssetPtr request = m_factoryRequestGetAsset->createObject( _doc );

        request->setLogin( _login );
        request->setPassword( _password );
        request->setContent( _content );

        return request;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestInterfacePtr AndroidHttpSystem::createHttpRequestDeleteMessage( const DocumentInterfacePtr & _doc )
    {
        AndroidHttpRequestDeleteMessagePtr request = m_factoryRequestDeleteMessage->createObject( _doc );

        return request;
    }
    //////////////////////////////////////////////////////////////////////////
}