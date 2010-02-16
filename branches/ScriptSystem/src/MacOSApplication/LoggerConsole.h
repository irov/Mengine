#	pragma once

#	include "Interface/FileSystemInterface.h"

namespace Menge
{
	class LoggerConsole
		: public OutputStreamInterface
	{
	public:
		LoggerConsole();
		virtual ~LoggerConsole();

		void write( const void* _data, int _count ) override;
		void flush() override;
	};
}	// namespace Menge