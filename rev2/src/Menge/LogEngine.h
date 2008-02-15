#	pragma once

#	include "Interface/LogSystemInterface.h"

#	include "Holder.h"

#	include <string>

namespace Menge
{
	class LogEngine
	{
	public:
		LogEngine( LogSystemInterface * _interface );

	public:
		void logMessage( const std::string & _message, bool _maskDebug );

	protected:
		LogSystemInterface * m_interface;
	};

	class LoggerOperator
	{
	public:
		LoggerOperator( const char * _file, bool _maskDebug );

	public:
		void operator()( const char * _message, ... );

	protected:
		const char * m_file;
		bool m_maskDebug;
	};
}

#	define MENGE_LOG\
	Menge::LoggerOperator( __FILE__, false )

#	define MENGE_LOG_DEBUG\
	Menge::LoggerOperator( __FILE__, true )
