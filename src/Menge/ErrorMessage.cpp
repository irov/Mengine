#	include "ErrorMessage.h"

#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#	include <stdio.h>

#	include <sstream>

namespace NError
{
	void CMessageError::operator()(const char *Message, ...)
	{
		va_list argList;
		//Initialize variable argument list
		va_start(argList, Message);

		//Write the error to the log file

		char str[1024];

		vsprintf_s( str, Message, argList );

		va_end(argList);
		std::stringstream StrMessage;
		StrMessage << m_File << " [" << m_Line << "] " << std::endl << str;
		if( MessageBox(0,StrMessage.str().c_str(),0,MB_OKCANCEL) == IDCANCEL )
		{
			PostQuitMessage (0);
		}
	}

	void CExceptionError::operator()(const char *Message, ...)
	{
		va_list argList;
		//Initialize variable argument list
		va_start(argList, Message);

		//Write the error to the log file

		char str[1024];

		vsprintf_s( str, Message, argList );

		va_end(argList);
		std::stringstream StrMessage;
		StrMessage << m_File << " [" << m_Line << "] " << std::endl << str;

		throw Data(StrMessage.str());
	}
}
