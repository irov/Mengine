#	pragma once

#	include "Interface/LogSystemInterface.h"
#	include "WindowsIncluder.h"

namespace Menge
{
	class LoggerConsole
		: public LoggerInterface
	{
	public:
		LoggerConsole();
		~LoggerConsole();

	public:
		void createConsole();

	public:
		void log( const void* _data, int _count, EMessageLevel _level ) override;
		

	private:
		bool m_createConsole;
		typedef std::vector<wchar_t> TWCharVector;
		TWCharVector m_wBuffer;
		typedef std::vector<char> TACharVector;
		TACharVector m_aBuffer;
		HANDLE m_ConsoleHandle;
	};
}	// namespace Menge
