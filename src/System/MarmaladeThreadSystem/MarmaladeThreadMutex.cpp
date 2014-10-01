#   include "MarmaladeThreadMutex.h"

#	include "Logger/Logger.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    MarmaladeThreadMutex::MarmaladeThreadMutex()
        : m_serviceProvider(nullptr)
        , m_cs(nullptr)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeThreadMutex::initialize( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;

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
        s3eResult err = s3eThreadLockAcquire( m_cs );

		if( err != S3E_RESULT_SUCCESS )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeThreadMutex::lock invalid lock %d"
				, err
				);
		}
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeThreadMutex::unlock()
    {
        s3eResult err = s3eThreadLockRelease( m_cs );

		if( err != S3E_RESULT_SUCCESS )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeThreadMutex::unlock invalid lock %d"
				, err
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
				LOGGER_ERROR(m_serviceProvider)("MarmaladeThreadMutex::lock invalid lock %d"
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