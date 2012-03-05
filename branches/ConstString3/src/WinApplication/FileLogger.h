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
		void setVerboseLevel( EMessageLevel _level ) override;
		bool validVerboseLevel( EMessageLevel _level ) const override;

	public:
		void setFileInterface( FileOutputStreamInterface* _fileInterface );
		FileOutputStreamInterface* getFileInterface();

	protected:
		void log( const char * _data, int _count, EMessageLevel _level ) override;
		void flush() override;

	protected:
		EMessageLevel m_verboseLevel;

		FileOutputStreamInterface* m_fileOutIterface;
	};
}
