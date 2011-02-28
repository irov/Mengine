#	pragma once

#	include "Interface/LogSystemInterface.h"


namespace Menge
{
	class FileOutputInterface;

	class FileLogger
		: public LoggerInterface
	{
	public:
		FileLogger();

	public:
		void setFileInterface( FileOutputInterface* _fileInterface );
		FileOutputInterface* getFileInterface();

	protected:
		void log( const void* _data, int _count, EMessageLevel _level ) override;
		void flush() override;

	protected:
		FileOutputInterface* m_fileOutIterface;
	};
}
