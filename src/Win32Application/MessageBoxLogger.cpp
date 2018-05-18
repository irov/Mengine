#include "MessageBoxLogger.h"

#include "WIN32/WindowsIncluder.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	MessageBoxLogger::MessageBoxLogger()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MessageBoxLogger::~MessageBoxLogger()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void MessageBoxLogger::log( EMessageLevel _level, uint32_t _flag, const Char * _data, size_t _count )
	{	
		(void)_level;
        (void)_flag;
		(void)_count;
		
		MessageBoxA( NULL, _data, "Critical Error", MB_OK );
	}
}	
