#include "cURLHttpSystem.h"

#include "cURLHttpRequestGetMessage.h"
#include "cURLHttpRequestPostMessage.h"
#include "cURLHttpRequestHeaderData.h"
#include "cURLHttpRequestGetAsset.h"
#include "cURLHttpRequestDeleteMessage.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/HttpLogger.h"
#include "Kernel/AssertionFactory.h"

#include "Config/StdString.h"

#include "curl/curl.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( HttpSystem, Mengine::cURLHttpSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void * curl_malloc( size_t _size )
        {
            void * p = Helper::allocateMemory( _size, "curl" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static void curl_free( void * _ptr )
        {
            Helper::deallocateMemory( _ptr, "curl" );
        }
        //////////////////////////////////////////////////////////////////////////
        static void * curl_realloc( void * _ptr, size_t _size )
        {
            void * p = Helper::reallocateMemory( _ptr, _size, "curl" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static char * curl_strdup( const char * str )
        {
            size_t len = MENGINE_STRLEN( str ) + 1;

            void * p = Helper::allocateMemory( len, "curl" );

            if( p == nullptr )
            {
                return nullptr;
            }

            return (char *)MENGINE_MEMCPY( p, str, len );
        }
        //////////////////////////////////////////////////////////////////////////
        static void * curl_calloc( size_t _nmemb, size_t _size )
        {
            void * p = Helper::callocateMemory( _nmemb, _size, "curl" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    cURLHttpSystem::cURLHttpSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    cURLHttpSystem::~cURLHttpSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLHttpSystem::_initializeService()
    {
        CURLcode code = curl_global_init_mem( CURL_GLOBAL_ALL
            , &Detail::curl_malloc
            , &Detail::curl_free
            , &Detail::curl_realloc
            , &Detail::curl_strdup
            , &Detail::curl_calloc );

        if( code != CURLE_OK )
        {
            LOGGER_HTTP_ERROR( "invalid initialize curl '%s' [%u]"
                , curl_easy_strerror( code )
                , code
            );

            return false;
        }

        curl_version_info_data * info = curl_version_info( CURLVERSION_NOW );

        MENGINE_UNUSED( info );

        LOGGER_HTTP_MESSAGE( "cURL version: %s host: %s ssl: %s"
            , info->version
            , info->host
            , info->ssl_version
        );

        m_factoryTaskGetMessage = Helper::makeFactoryPool<cURLHttpRequestGetMessage, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskPostMessage = Helper::makeFactoryPool<cURLHttpRequestPostMessage, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskDeleteMessage = Helper::makeFactoryPool<cURLHttpRequestDeleteMessage, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskHeaderData = Helper::makeFactoryPool<cURLHttpRequestHeaderData, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTaskDownloadAsset = Helper::makeFactoryPool<cURLHttpRequestGetAsset, 16>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLHttpSystem::_finalizeService()
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

        curl_global_cleanup();
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestInterfacePtr cURLHttpSystem::createHttpRequestGetMessage( const DocumentInterfacePtr & _doc )
    {
        cURLHttpRequestGetMessagePtr task = m_factoryTaskGetMessage->createObject( _doc );

        return task;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestInterfacePtr cURLHttpSystem::createHttpRequestPostMessage( const HttpRequestPostProperties & _params, const DocumentInterfacePtr & _doc )
    {
        cURLHttpRequestPostMessagePtr task = m_factoryTaskPostMessage->createObject( _doc );

        task->setPostProperties( _params );

        return task;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestInterfacePtr cURLHttpSystem::createHttpRequestHeaderData( const Data & _data, const DocumentInterfacePtr & _doc )
    {
        cURLHttpRequestHeaderDataPtr task = m_factoryTaskHeaderData->createObject( _doc );

        task->setData( _data );

        return task;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestInterfacePtr cURLHttpSystem::createHttpRequestDownloadAsset( const String & _login, const String & _password, const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc )
    {
        cURLHttpRequestGetAssetPtr task = m_factoryTaskDownloadAsset->createObject( _doc );

        task->setLogin( _login );
        task->setPassword( _password );
        task->setContent( _content );

        return task;
    }
    //////////////////////////////////////////////////////////////////////////
    HttpRequestInterfacePtr cURLHttpSystem::createHttpRequestDeleteMessage( const DocumentInterfacePtr & _doc )
    {
        cURLHttpRequestDeleteMessagePtr task = m_factoryTaskDeleteMessage->createObject( _doc );

        return task;
    }
    //////////////////////////////////////////////////////////////////////////
}