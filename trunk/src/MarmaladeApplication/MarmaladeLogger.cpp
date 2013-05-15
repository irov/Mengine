
#	include "MarmaladeLogger.h"


namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
	MarmaladeLogger::MarmaladeLogger()
	{
    }
    //////////////////////////////////////////////////////////////////////////
	void MarmaladeLogger::log( EMessageLevel _level, size_t _flag, const char * _data, size_t _count )
	{
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
    void MarmaladeLogger::setVerboseFlag( size_t _flag )
    {
        (void)_flag;
    }
    //////////////////////////////////////////////////////////////////////////
	bool MarmaladeLogger::validMessage( EMessageLevel _level, size_t _flag ) const
	{
        (void)_level;
        (void)_flag;

        return true;
	}
}
