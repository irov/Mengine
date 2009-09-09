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

	private:
		void utf8_to_ansi_( const char* _utf8, int _size, std::string* _ansiString );

	private:
		bool m_createConsole;
		typedef std::vector<wchar_t> TWCharVector;
		TWCharVector m_wBuffer;
		typedef std::vector<char> TACharVector;
		TACharVector m_aBuffer;
	};
}	// namespace Menge
