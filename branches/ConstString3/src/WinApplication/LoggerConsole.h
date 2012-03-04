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
		void log( const char * _data, int _count, EMessageLevel _level ) override;
		void flush() override;

	private:
		bool m_createConsole;
		
		typedef std::vector<wchar_t> TWCharVector;
		TWCharVector m_wBuffer;

		typedef std::vector<char> TACharVector;
		TACharVector m_aBuffer;

		HANDLE m_ConsoleHandle;
		
		FILE *fp[3];
		HANDLE lStdHandle[3];
		int hConHandle[3];
	};
}	// namespace Menge
