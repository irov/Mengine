#	include "MarmaladeLogger.h"

#   include "s3eDebug.h"
#   include "s3eDevice.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
	MarmaladeLogger::MarmaladeLogger()
		: m_verboseLevel(LM_INFO)
		, m_verboseFlag(0xFFFFFFFF)
	{
    }
	//////////////////////////////////////////////////////////////////////////
	MarmaladeLogger::~MarmaladeLogger()
	{
	}
    //////////////////////////////////////////////////////////////////////////
	void MarmaladeLogger::log( EMessageLevel _level, uint32_t _flag, const char * _data, size_t _size )
	{
		(void)_level;
		(void)_flag;
		
		printf( "%s", _data );

		if( _level == LM_CRITICAL )
		{
			s3eDebugErrorShow( S3E_MESSAGE_CONTINUE_STOP, _data );
		}
		else if( _level == LM_FATAL )
		{
			s3eDebugErrorShow( S3E_MESSAGE_CONTINUE_STOP, _data );
		}
	}
    //////////////////////////////////////////////////////////////////////////
	void MarmaladeLogger::flush()
	{
		//fflush(stdout);
	}
    //////////////////////////////////////////////////////////////////////////
	void MarmaladeLogger::setVerboseLevel( EMessageLevel _level )
	{
        m_verboseLevel = _level;
	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeLogger::setVerboseFlag( uint32_t _flag )
    {
        m_verboseFlag = _flag;
    }
    //////////////////////////////////////////////////////////////////////////
	bool MarmaladeLogger::validMessage( EMessageLevel _level, uint32_t _flag ) const
	{
		if( m_verboseLevel < _level )
		{
			return false;
		}

		if( _flag == 0 )
		{
			return true;
		}

		if( (m_verboseFlag & _flag) == 0 )
		{
			return false;
		}

        return true;
	}
}
