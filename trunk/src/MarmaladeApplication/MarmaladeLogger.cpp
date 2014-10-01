#	include "MarmaladeLogger.h"

#   include "s3eDebug.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
	MarmaladeLogger::MarmaladeLogger()
	{
    }
    //////////////////////////////////////////////////////////////////////////
	void MarmaladeLogger::log( EMessageLevel _level, uint32_t _flag, const char * _data, size_t _size )
	{
		//s3eDebugErrorPrintf("%s"
  //          , _data 
  //          );
		printf( _data );

		if( _level == LM_CRITICAL )
		{
			s3eDebugErrorShow( S3E_MESSAGE_CONTINUE_STOP, _data );
		}

		if( _level == LM_FATAL )
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
        (void)_level;
	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeLogger::setVerboseFlag( uint32_t _flag )
    {
        (void)_flag;
    }
    //////////////////////////////////////////////////////////////////////////
	bool MarmaladeLogger::validMessage( EMessageLevel _level, uint32_t _flag ) const
	{
        (void)_level;
        (void)_flag;

        return true;
	}
}
