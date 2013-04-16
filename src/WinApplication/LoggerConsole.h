#	pragma once

#	include "Interface/LogSystemInterface.h"

#	include "WindowsLayer/WindowsIncluder.h"

namespace Menge
{
	class LoggerConsole
		: public LoggerInterface
	{
	public:
		LoggerConsole( ServiceProviderInterface * _serviceProvider );
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
        ServiceProviderInterface * m_serviceProvider;

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
