#	pragma once

#	include "Interface/LogSystemInterface.h"
#	include "WinIncluder.h"

namespace Menge
{
	class FileOutputStreamInterface;

	class ParticlesLogger
		: public LoggerInterface
	{
	public:
		ParticlesLogger();
		~ParticlesLogger();

	public:
		
		bool initialize( FileOutputStreamInterface* _fileInterface, bool _console );
	public:
		void log( const char * _data, int _count, EMessageLevel _level ) override;
		void flush() override;
				
		FileOutputStreamInterface* getFileInterface();
	private:
		void createConsole_();
		void writeToConsole_(const char * _data, int _count, EMessageLevel _level);
		
		void setVerboseLevel( EMessageLevel _level ) override;
		bool validVerboseLevel( EMessageLevel _level ) const override;
		
		typedef std::vector<wchar_t> TWCharVector;
		TWCharVector m_wBuffer;
		bool m_createConsole;
		typedef std::vector<char> TACharVector;
		TACharVector m_aBuffer;

		HANDLE m_ConsoleHandle;
		FILE *fp[3];
		HANDLE lStdHandle[3];
		int hConHandle[3];

		FileOutputStreamInterface* m_fileOutIterface;
	};
}	// namespace Menge
