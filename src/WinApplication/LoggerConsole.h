#	pragma once

#	include "Interface/FileSystemInterface.h"

namespace Menge
{
	class LoggerConsole
		: public OutputStreamInterface
	{
	public:
		LoggerConsole();
		~LoggerConsole();

	public:
		void createConsole();

	public:
		void write( const void* _data, int _count ) override;
		void flush() override;

	private:
		bool m_createConsole;
		typedef std::vector<wchar_t> TWCharVector;
		TWCharVector m_wBuffer;
		typedef std::vector<char> TACharVector;
		TACharVector m_aBuffer;
	};
}	// namespace Menge
