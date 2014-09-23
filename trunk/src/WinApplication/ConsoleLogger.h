#	pragma once

#	include "Interface/LogSystemInterface.h"

#	include "WindowsLayer/WindowsIncluder.h"

namespace Menge
{
	class ConsoleLogger
		: public LoggerInterface
	{
	public:
		ConsoleLogger( ServiceProviderInterface * _serviceProvider );
		~ConsoleLogger();

	public:
		void createConsole();

	public:
		void setVerboseLevel( EMessageLevel _level ) override;
        void setVerboseFlag( uint32_t _flag ) override;

    public:
		bool validMessage( EMessageLevel _level, uint32_t _flag ) const override;

	public:
		void log( EMessageLevel _level, uint32_t _flag, const char * _data, size_t _count ) override;
		void flush() override;

	private:
        ServiceProviderInterface * m_serviceProvider;

		EMessageLevel m_verboseLevel;
        uint32_t m_verboseFlag;

		bool m_createConsole;
		
		TVectorWChar m_wBuffer;
		TVectorChar m_aBuffer;

		HANDLE m_ConsoleHandle;
		
		FILE *fp[3];
		HANDLE lStdHandle[3];
		int hConHandle[3];
	};
}
