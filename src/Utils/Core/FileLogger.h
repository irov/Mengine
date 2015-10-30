#	pragma once

#	include "Interface/LoggerInterface.h"
#	include "Interface/StreamInterface.h"

namespace Menge
{
	class FileLogger
		: public LoggerInterface
	{
	public:
		FileLogger();
		virtual ~FileLogger();

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
		OutputStreamInterfacePtr getStream() const;

	protected:
		void log( EMessageLevel _level, uint32_t _flag, const char * _data, size_t _count ) override;

    protected:
		void flush() override;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		EMessageLevel m_verboseLevel;
        uint32_t m_verboseFlag;

		OutputStreamInterfacePtr m_stream;
	};
}
