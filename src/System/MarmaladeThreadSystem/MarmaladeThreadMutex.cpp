#   include "MarmaladeThreadMutex.h"

#	include "Logger/Logger.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    MarmaladeThreadMutex::MarmaladeThreadMutex()
        : m_serviceProvider(nullptr)
        , m_cs(nullptr)
		, m_doc(nullptr)
    {
    }
    //////////////////////////////////////////////////////////////////////////
	bool MarmaladeThreadMutex::initialize( ServiceProviderInterface * _serviceProvider, const char * _doc )
    {
        m_serviceProvider = _serviceProvider;

		m_doc = _doc;

        s3eThreadLock * cs = s3eThreadLockCreate();

		if( cs == nullptr )
		{
			return false;
		}

		m_cs = cs;

		return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeThreadMutex::lock()
    {
        s3eResult err = s3eThreadLockAcquire( m_cs, -1 );

		if( err != S3E_RESULT_SUCCESS )
		{
			s3eThreadError thread_err = s3eThreadGetError();
			const char * thread_err_str = s3eThreadGetErrorString();

			LOGGER_ERROR(m_serviceProvider)("MarmaladeThreadMutex::lock invalid lock %s:%d"
				, thread_err_str
				, thread_err
				);
		}
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeThreadMutex::unlock()
    {
        s3eResult err = s3eThreadLockRelease( m_cs );

		if( err != S3E_RESULT_SUCCESS )
		{
			s3eThreadError thread_err = s3eThreadGetError();
			const char * thread_err_str = s3eThreadGetErrorString();

			LOGGER_ERROR(m_serviceProvider)("MarmaladeThreadMutex::unlock invalid lock %s:%d"
				, thread_err_str
				, thread_err
				);
		}
    }
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeThreadMutex::try_lock()
	{
		s3eResult err = s3eThreadLockAcquire( m_cs, 0 );

		if( err == S3E_RESULT_SUCCESS )
		{
			return true;
		}
		else if( err == S3E_RESULT_ERROR )
		{
			s3eThreadError thread_err = s3eThreadGetError();

			if( thread_err == S3E_THREAD_ERR_TIMEOUT )
			{
				return false;
			}
			else
			{
				const char * thread_err_str = s3eThreadGetErrorString();

				LOGGER_ERROR(m_serviceProvider)("MarmaladeThreadMutex::lock invalid lock %s code %d"
					, thread_err_str
					, thread_err
					);
			}
		}

		return false;
	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeThreadMutex::_destroy()
    {
        s3eThreadLockDestroy( m_cs );
    }
}