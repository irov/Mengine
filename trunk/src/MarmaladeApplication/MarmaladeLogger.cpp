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
        printf("%s"
            , _data
            );
	}
    //////////////////////////////////////////////////////////////////////////
	void MarmaladeLogger::flush()
	{
		fflush(stdout);
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
