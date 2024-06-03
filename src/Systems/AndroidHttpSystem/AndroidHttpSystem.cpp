#include "AndroidHttpSystem.h"

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
        m_factoryTaskGetMessage = Helper::makeFactoryPool<AndroidHttpRequestGetMessage, 16>(MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskPostMessage = Helper::makeFactoryPool<AndroidHttpRequestPostMessage, 16>(MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskDeleteMessage = Helper::makeFactoryPool<AndroidHttpRequestDeleteMessage, 16>(MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskHeaderData = Helper::makeFactoryPool<AndroidHttpRequestHeaderData, 16>(MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskGetAsset = Helper::makeFactoryPool<AndroidHttpRequestGetAsset, 16>(MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidHttpSystem::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY(m_factoryTaskGetAsset );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTaskPostMessage );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTaskDeleteMessage );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTaskHeaderData );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTaskGetMessage );

        m_factoryTaskGetAsset = nullptr;
        m_factoryTaskPostMessage = nullptr;
        m_factoryTaskDeleteMessage = nullptr;
        m_factoryTaskHeaderData = nullptr;
        m_factoryTaskGetMessage = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestInterfacePtr AndroidHttpSystem::createHttpRequestGetMessage( const DocumentInterfacePtr & _doc )
    {
        AndroidHttpRequestGetMessagePtr task = m_factoryTaskGetMessage->createObject(_doc );

        return task;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestInterfacePtr AndroidHttpSystem::createHttpRequestPostMessage( const HttpRequestPostProperties & _params, const DocumentInterfacePtr & _doc )
    {
        AndroidHttpRequestPostMessagePtr task = m_factoryTaskPostMessage->createObject(_doc );

        task->setPostProperties( _params );

        return task;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestInterfacePtr AndroidHttpSystem::createHttpRequestHeaderData( const Data & _data, const DocumentInterfacePtr & _doc )
    {
        AndroidHttpRequestHeaderDataPtr task = m_factoryTaskHeaderData->createObject(_doc );

        task->setData( _data );

        return task;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestInterfacePtr AndroidHttpSystem::createHttpRequestGetAsset( const String & _login, const String & _password, const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc )
    {
        AndroidHttpRequestGetAssetPtr task = m_factoryTaskGetAsset->createObject(_doc );

        task->setLogin( _login );
        task->setPassword( _password );
        task->setContent( _content );

        return task;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestInterfacePtr AndroidHttpSystem::createHttpRequestDeleteMessage( const DocumentInterfacePtr & _doc )
    {
        AndroidHttpRequestDeleteMessagePtr task = m_factoryTaskDeleteMessage->createObject(_doc );

        return task;
    }
    //////////////////////////////////////////////////////////////////////////
}