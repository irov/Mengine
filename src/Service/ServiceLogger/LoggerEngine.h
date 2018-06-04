#pragma once

#include "Interface/LoggerInterface.h"

#include "Core/ServiceBase.h"

#include "Logger/Logger.h"

#include "Config/Vector.h"

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
		void logMessage( EMessageLevel _level, uint32_t _flag, const char * _message, uint32_t _size  ) override;
        uint32_t getCountMessage( EMessageLevel _level ) override;

	public:
		bool registerLogger( const LoggerInterfacePtr & _logger ) override;
		bool unregisterLogger( const LoggerInterfacePtr & _logger ) override;
	
	protected:
		EMessageLevel m_verboseLevel;
        uint32_t m_verboseFlag;

		typedef Vector<LoggerInterfacePtr> TVectorLoggers;
		TVectorLoggers m_loggers;

        uint32_t m_countMessage[LM_MAX];
	};
}