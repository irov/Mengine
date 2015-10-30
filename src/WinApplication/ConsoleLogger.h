#	pragma once

#	include "Interface/LoggerInterface.h"

#	include "WindowsLayer/WindowsIncluder.h"

#	include "Config/String.h"

namespace Menge
{
	class ConsoleLogger
		: public LoggerInterface
	{
	public:
		ConsoleLogger();
		~ConsoleLogger();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool initialize() override;
		void finalize() override;

	public:
		void setVerboseLevel( EMessageLevel _level ) override;
        void setVerboseFlag( uint32_t _flag ) override;

    public:
		bool validMessage( EMessageLevel _level, uint32_t _flag ) const override;

	public:
		void log( EMessageLevel _level, uint32_t _flag, const char * _data, size_t _count ) override;
		void flush() override;

	protected:
		void createConsole_();
		void removeConsole_();

	private:
		ServiceProviderInterface * m_serviceProvider;

		EMessageLevel m_verboseLevel;
        uint32_t m_verboseFlag;

		bool m_createConsole;
		
		TVectorWChar m_wBuffer;
		TVectorChar m_aBuffer;

		HANDLE m_ConsoleHandle;
		
		FILE * m_fp[3];
		HANDLE m_lStdHandle[3];
		int m_hConHandle[3];
		fpos_t m_pOldHandle[3];
		int m_hOldHandle[3];
	};
}
