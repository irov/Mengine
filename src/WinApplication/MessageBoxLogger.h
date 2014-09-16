#	pragma once

#	include "Interface/LogSystemInterface.h"

namespace Menge
{
	class MessageBoxLogger
		: public LoggerInterface
	{
	public:
		MessageBoxLogger( ServiceProviderInterface * _serviceProvider );
		~MessageBoxLogger();

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
	};
}
