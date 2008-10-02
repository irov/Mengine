#	pragma once

#	include "Interface/LogSystemInterface.h"

namespace Menge
{
	class LoggerConsole
		: public LoggerInterface
	{
	public:
		LoggerConsole();
		virtual ~LoggerConsole();

		void outputMessage( const StringA& _message ) override;

	protected:
		bool m_createConsole;
	};
}	// namespace Menge