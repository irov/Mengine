#pragma once

#include "Core/ServantBase.h"
#include "Core/LoggerBase.h"

namespace Mengine
{
	class MessageBoxLogger
		: public LoggerBase
	{
	public:
		MessageBoxLogger();
		~MessageBoxLogger() override;

	public:
		void log( EMessageLevel _level, uint32_t _flag, const Char * _data, size_t _count ) override;
	};
}
