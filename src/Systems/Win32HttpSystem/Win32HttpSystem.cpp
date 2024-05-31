#include "Win32HttpSystem.h"

#include "Win32HttpRequestGetMessage.h"
#include "Win32HttpRequestPostMessage.h"
#include "Win32HttpRequestHeaderData.h"
#include "Win32HttpRequestGetAsset.h"
#include "Win32HttpRequestDeleteMessage.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/HttpLogger.h"
#include "Kernel/AssertionFactory.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( HttpSystem, Mengine::Win32HttpSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32HttpSystem::Win32HttpSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32HttpSystem::~Win32HttpSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32HttpSystem::_initializeService()
    {
        m_factoryTaskGetMessage = Helper::makeFactoryPool<Win32HttpRequestGetMessage, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskPostMessage = Helper::makeFactoryPool<Win32HttpRequestPostMessage, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskDeleteMessage = Helper::makeFactoryPool<Win32HttpRequestDeleteMessage, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskHeaderData = Helper::makeFactoryPool<Win32HttpRequestHeaderData, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskDownloadAsset = Helper::makeFactoryPool<Win32HttpRequestGetAsset, 16>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32HttpSystem::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTaskDownloadAsset );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTaskPostMessage );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTaskDeleteMessage );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTaskHeaderData );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTaskGetMessage );

        m_factoryTaskDownloadAsset = nullptr;
        m_factoryTaskPostMessage = nullptr;
        m_factoryTaskDeleteMessage = nullptr;
        m_factoryTaskHeaderData = nullptr;
        m_factoryTaskGetMessage = nullptr;
    }    
    //////////////////////////////////////////////////////////////////////////
    HttpRequestInterfacePtr Win32HttpSystem::createHttpRequestGetMessage( const DocumentInterfacePtr & _doc )
    {
        Win32HttpRequestGetMessagePtr task = m_factoryTaskGetMessage->createObject( _doc );

        return task;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestInterfacePtr Win32HttpSystem::createHttpRequestPostMessage( const HttpRequestPostProperties & _params, const DocumentInterfacePtr & _doc )
    {
        Win32HttpRequestPostMessagePtr task = m_factoryTaskPostMessage->createObject( _doc );

        task->setPostProperties( _params );

        return task;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestInterfacePtr Win32HttpSystem::createHttpRequestHeaderData( const Data & _data, const DocumentInterfacePtr & _doc )
    {
        Win32HttpRequestHeaderDataPtr task = m_factoryTaskHeaderData->createObject( _doc );

        task->setData( _data );

        return task;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestInterfacePtr Win32HttpSystem::createHttpRequestDownloadAsset( const String & _login, const String & _password, const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc )
    {
        Win32HttpRequestGetAssetPtr task = m_factoryTaskDownloadAsset->createObject( _doc );

        task->setLogin( _login );
        task->setPassword( _password );
        task->setContent( _content );

        return task;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestInterfacePtr Win32HttpSystem::createHttpRequestDeleteMessage( const DocumentInterfacePtr & _doc )
    {
        Win32HttpRequestDeleteMessagePtr task = m_factoryTaskDeleteMessage->createObject( _doc );

        return task;
    }
    //////////////////////////////////////////////////////////////////////////
}
