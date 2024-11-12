#include "AppleHttpSystem.h"

#include "AppleHttpRequestPing.h"
#include "AppleHttpRequestGetMessage.h"
#include "AppleHttpRequestPostMessage.h"
#include "AppleHttpRequestHeaderData.h"
#include "AppleHttpRequestGetAsset.h"
#include "AppleHttpRequestDeleteMessage.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/HttpLogger.h"
#include "Kernel/AssertionFactory.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( HttpSystem, Mengine::AppleHttpSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleHttpSystem::AppleHttpSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleHttpSystem::~AppleHttpSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleHttpSystem::_initializeService()
    {
        m_factoryRequestPing = Helper::makeFactoryPool<AppleHttpRequestPing, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRequestGetMessage = Helper::makeFactoryPool<AppleHttpRequestGetMessage, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRequestPostMessage = Helper::makeFactoryPool<AppleHttpRequestPostMessage, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRequestDeleteMessage = Helper::makeFactoryPool<AppleHttpRequestDeleteMessage, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRequestHeaderData = Helper::makeFactoryPool<AppleHttpRequestHeaderData, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryRequestGetAsset = Helper::makeFactoryPool<AppleHttpRequestGetAsset, 16>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleHttpSystem::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRequestPing );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRequestGetAsset );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRequestPostMessage );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRequestDeleteMessage );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRequestHeaderData );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRequestGetMessage );

        m_factoryRequestPing = nullptr;
        m_factoryTaskGetAsset = nullptr;
        m_factoryTaskPostMessage = nullptr;
        m_factoryTaskDeleteMessage = nullptr;
        m_factoryTaskHeaderData = nullptr;
        m_factoryTaskGetMessage = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestInterfacePtr AppleHttpSystem::createHttpRequestPing( const DocumentInterfacePtr & _doc )
    {
        AppleHttpRequestPingPtr task = m_factoryRequestPing->createObject( _doc );

        return task;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestInterfacePtr AppleHttpSystem::createHttpRequestGetMessage( const DocumentInterfacePtr & _doc )
    {
        AppleHttpRequestGetMessagePtr task = m_factoryTaskGetMessage->createObject( _doc );

        return task;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestInterfacePtr AppleHttpSystem::createHttpRequestPostMessage( const HttpRequestPostProperties & _params, const DocumentInterfacePtr & _doc )
    {
        AppleHttpRequestPostMessagePtr task = m_factoryTaskPostMessage->createObject( _doc );

        task->setPostProperties( _params );

        return task;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestInterfacePtr AppleHttpSystem::createHttpRequestHeaderData( const Data & _data, const DocumentInterfacePtr & _doc )
    {
        AppleHttpRequestHeaderDataPtr task = m_factoryTaskHeaderData->createObject( _doc );

        task->setData( _data );

        return task;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestInterfacePtr AppleHttpSystem::createHttpRequestGetAsset( const String & _login, const String & _password, const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc )
    {
        AppleHttpRequestGetAssetPtr task = m_factoryTaskGetAsset->createObject( _doc );

        task->setLogin( _login );
        task->setPassword( _password );
        task->setContent( _content );

        return task;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestInterfacePtr AppleHttpSystem::createHttpRequestDeleteMessage( const DocumentInterfacePtr & _doc )
    {
        AppleHttpRequestDeleteMessagePtr task = m_factoryTaskDeleteMessage->createObject( _doc );

        return task;
    }
    //////////////////////////////////////////////////////////////////////////
}
