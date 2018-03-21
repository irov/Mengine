#pragma once

#include "Interface/LoggerInterface.h"

#include "Core/ServiceBase.h"

#include "Logger/Logger.h"

#include "stdex/stl_vector.h"

namespace Mengine
{
	class LoggerEngine
		: public ServiceBase<LoggerServiceInterface>
	{
	public:
		LoggerEngine();
		~LoggerEngine() override;

	public:
		void setVerboseLevel( EMessageLevel _level ) override;
        void setVerboseFlag( uint32_t _flag ) override;

    public:
		bool validMessage( EMessageLevel _level, uint32_t _flag ) const override;

	public:
		void logMessage( EMessageLevel _level, uint32_t _flag, const char * _message, size_t _size  ) override;
		size_t getCountMessage( Mengine::EMessageLevel _level ) override;

	public:
		bool registerLogger( const LoggerInterfacePtr & _logger ) override;
		bool unregisterLogger( const LoggerInterfacePtr & _logger ) override;
	
	protected:
		Mengine::EMessageLevel m_verboseLevel;
        uint32_t m_verboseFlag;

		typedef stdex::vector<LoggerInterfacePtr> TVectorLoggers;
		TVectorLoggers m_loggers;

		size_t m_countMessage[LM_MAX];
	};
}