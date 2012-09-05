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
		void setVerboseLevel( EMessageLevel _level ) override;
		bool validVerboseLevel( EMessageLevel _level ) const override;

	public:
		void log( const char * _data, int _count, EMessageLevel _level ) override;
		void flush() override;

	private:
		EMessageLevel m_verboseLevel;

		bool m_createConsole;
		
		typedef std::vector<WChar> TVectorWChar;
		TVectorWChar m_wBuffer;

		typedef std::vector<Char> TVectorChar;
		TVectorChar m_aBuffer;

		HANDLE m_ConsoleHandle;
		
		FILE *fp[3];
		HANDLE lStdHandle[3];
		int hConHandle[3];
	};
}	// namespace Menge
