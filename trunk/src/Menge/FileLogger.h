#	pragma once

#	include "Interface/LogSystemInterface.h"


namespace Menge
{
	class FileOutputInterface;

	class FileLogger
		:public LoggerInterface
	{
	private:
		FileOutputInterface* m_fileOutIterface;

	public:
		FileLogger();
		void setFileInterface(FileOutputInterface* _fileInterface);
		FileOutputInterface* getFileInterface();
		void log( const void* _data, int _count, EMessageLevel _level );
	};

}
