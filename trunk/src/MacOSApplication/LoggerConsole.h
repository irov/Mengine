#	pragma once

#	include "Interface/FileSystemInterface.h"

namespace Menge
{
	class LoggerConsole
		: public OutStreamInterface
	{
	public:
		LoggerConsole();
		virtual ~LoggerConsole();

		void write( const void* _data, std::streamsize _count ) override;
		void write( const String& _str ) override;
		void write( int _num ) override;
		void write( float _num ) override;
		void flush() override;
	};
}	// namespace Menge