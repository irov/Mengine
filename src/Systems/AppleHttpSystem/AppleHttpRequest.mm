#include "AppleHttpRequest.h"

#import "Environment/Apple/AppleString.h"
#import "Environment/Apple/AppleDetail.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/ProfilerHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Config/StdIntTypes.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleHttpRequest::AppleHttpRequest()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleHttpRequest::~AppleHttpRequest()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleHttpRequest::_onThreadTaskPreparation()
    {
        ThreadMutexInterfacePtr mutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex );

        m_mutex = mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleHttpRequest::_onThreadTaskRun()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleHttpRequest::_onThreadTaskProcess()
    {
        MENGINE_PROFILER_CATEGORY();

        MENGINE_ASSERTION_MEMORY_PANIC( m_response, "not setup 'response'" );
        MENGINE_ASSERTION_MEMORY_PANIC( m_receiver, "not setup 'receiver'" );

        MengineHttpRequestParam * request = [MengineHttpRequestParam alloc];
        
        request.HTTP_URL = Helper::stringToNSString( m_url );
        request.HTTP_PROXY = Helper::stringToNSString( m_proxy );
        request.HTTP_HEADERS = Helper::AppleGetNSArrayFromVectorString( m_headers );
        request.HTTP_COOKIES = Helper::stringToNSString( m_cookies );
        request.HTTP_TIMEOUT = m_timeout;

        MengineHttpResponseParam * response = this->_onHttp( request );

        if( response == nil )
        {
            m_mutex->lock();

            if( m_response != nullptr )
            {
                m_response->setSuccessful( false );
            }

            m_mutex->unlock();

            return true;
        }

        m_mutex->lock();

        if( m_response != nullptr )
        {
            m_response->setSuccessful( true );

            m_response->setCode( (EHttpCode)response.HTTP_RESPONSE_CODE );

            if( response.HTTP_CONTENT_DATA != nullptr )
            {
                const Char * bytes = (const Char *)[response.HTTP_CONTENT_DATA bytes];
                m_response->appendData( bytes, response.HTTP_CONTENT_LENGTH );
            }

            if( response.HTTP_ERROR_MESSAGE != nullptr )
            {
                String error_message = Helper::NSStringToString( response.HTTP_ERROR_MESSAGE );
                int32_t error_code = (int32_t)response.HTTP_ERROR_CODE;
                
                m_response->setError( error_message, error_code );
            }
        }

        m_mutex->unlock();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleHttpRequest::_onThreadTaskComplete( bool _successful )
    {
        m_response->setSuccessful( _successful );

        m_mutex->lock();

        HttpResponseInterfacePtr response = std::move( m_response );
        HttpReceiverInterfacePtr receiver = std::move( m_receiver );
        receiver->onHttpRequestComplete( response );

        m_mutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
}
