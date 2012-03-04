#	pragma once

#	include "Interface/LogSystemInterface.h"


namespace Menge
{
	class FileOutputStreamInterface;

	class FileLogger
		: public LoggerInterface
	{
	public:
		FileLogger();

	public:
		void setFileInterface( FileOutputStreamInterface* _fileInterface );
		FileOutputStreamInterface* getFileInterface();

	protected:
		void log( const char * _data, int _count, EMessageLevel _level ) override;
		void flush() override;

	protected:
		FileOutputStreamInterface* m_fileOutIterface;
	};
}
