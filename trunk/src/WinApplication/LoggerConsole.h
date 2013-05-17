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
        void setVerboseFlag( size_t _flag ) override;

    public:
		bool validMessage( EMessageLevel _level, size_t _flag ) const override;

	public:
		void log( EMessageLevel _level, size_t _flag, const char * _data, size_t _count ) override;
		void flush() override;

	private:
        ServiceProviderInterface * m_serviceProvider;

		EMessageLevel m_verboseLevel;
        size_t m_verboseFlag;

		bool m_createConsole;
		
		TVectorWChar m_wBuffer;
		TVectorChar m_aBuffer;

		HANDLE m_ConsoleHandle;
		
		FILE *fp[3];
		HANDLE lStdHandle[3];
		int hConHandle[3];
	};
}	// namespace Menge
