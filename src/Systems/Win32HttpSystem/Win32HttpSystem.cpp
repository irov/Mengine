#include "Win32HttpSystem.h"

#include "Interface/EnvironmentServiceInterface.h"

#include "Win32HttpRequestGetMessage.h"
#include "Win32HttpRequestPostMessage.h"
#include "Win32HttpRequestHeaderData.h"
#include "Win32HttpRequestGetAsset.h"
#include "Win32HttpRequestDeleteMessage.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/HttpLogger.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/ArrayString.h"

#include "Config/Version.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( HttpSystem, Mengine::Win32HttpSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32HttpSystem::Win32HttpSystem()
        : m_hInternet( NULL )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32HttpSystem::~Win32HttpSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32HttpSystem::_initializeService()
    {
        ArrayString<1024> agent;
        agent.append( "Mengine" );
        agent.append( "/" );
        agent.append( MENGINE_ENGINE_VERSION_STRING );
        agent.append( " (" );

        Char osFamily[MENGINE_ENVIRONMENT_OS_FAMILY_MAXNAME + 1] = {'\0'};
        ENVIRONMENT_SERVICE()
            ->getOSFamily( osFamily );

        agent.append( osFamily );

        Char osVersion[MENGINE_ENVIRONMENT_OS_VERSION_MAXNAME + 1] = {'\0'};
        ENVIRONMENT_SERVICE()
            ->getOSVersion( osVersion );

        agent.append( " " );
        agent.append( osVersion );

        agent.append( ")" );

        const Char * agent_str = agent.c_str();

        DWORD dwFlags = 0;

        HINTERNET hInternet = ::InternetOpenA( agent_str
            , INTERNET_OPEN_TYPE_PRECONFIG
            , NULL
            , NULL
            , dwFlags );

        if( hInternet == NULL )
        {
            return false;
        }

        m_hInternet = hInternet;

        m_factoryTaskGetMessage = Helper::makeFactoryPool<Win32HttpRequestGetMessage, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskPostMessage = Helper::makeFactoryPool<Win32HttpRequestPostMessage, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskDeleteMessage = Helper::makeFactoryPool<Win32HttpRequestDeleteMessage, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskHeaderData = Helper::makeFactoryPool<Win32HttpRequestHeaderData, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskGetAsset = Helper::makeFactoryPool<Win32HttpRequestGetAsset, 16>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32HttpSystem::_finalizeService()
    {
        ::InternetCloseHandle( m_hInternet );
        m_hInternet = NULL;

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTaskGetAsset );
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
    HttpRequestInterfacePtr Win32HttpSystem::createHttpRequestGetMessage( const DocumentInterfacePtr & _doc )
    {
        Win32HttpRequestGetMessagePtr task = m_factoryTaskGetMessage->createObject( _doc );

        task->setHInternet( m_hInternet );

        return task;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestInterfacePtr Win32HttpSystem::createHttpRequestPostMessage( const HttpRequestPostProperties & _params, const DocumentInterfacePtr & _doc )
    {
        Win32HttpRequestPostMessagePtr task = m_factoryTaskPostMessage->createObject( _doc );

        task->setHInternet( m_hInternet );
        task->setPostProperties( _params );

        return task;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestInterfacePtr Win32HttpSystem::createHttpRequestHeaderData( const Data & _data, const DocumentInterfacePtr & _doc )
    {
        Win32HttpRequestHeaderDataPtr task = m_factoryTaskHeaderData->createObject( _doc );

        task->setHInternet( m_hInternet );
        task->setData( _data );

        return task;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestInterfacePtr Win32HttpSystem::createHttpRequestGetAsset( const String & _login, const String & _password, const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc )
    {
        Win32HttpRequestGetAssetPtr task = m_factoryTaskGetAsset->createObject( _doc );

        task->setHInternet( m_hInternet );
        task->setLogin( _login );
        task->setPassword( _password );
        task->setContent( _content );

        return task;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestInterfacePtr Win32HttpSystem::createHttpRequestDeleteMessage( const DocumentInterfacePtr & _doc )
    {
        Win32HttpRequestDeleteMessagePtr task = m_factoryTaskDeleteMessage->createObject( _doc );

        task->setHInternet( m_hInternet );

        return task;
    }
    //////////////////////////////////////////////////////////////////////////
}
