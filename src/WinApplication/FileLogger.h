#	pragma once

#	include "Interface/LogSystemInterface.h"


namespace Menge
{
	class OutputStreamInterface;

	class FileLogger
		: public LoggerInterface
	{
	public:
		FileLogger( OutputStreamInterface* _fileStream );

	public:
		void setVerboseLevel( EMessageLevel _level ) override;
		bool validVerboseLevel( EMessageLevel _level ) const override;

	public:
		OutputStreamInterface * getStream() const;

	protected:
		void log( const char * _data, int _count, EMessageLevel _level ) override;
		void flush() override;

	protected:
		EMessageLevel m_verboseLevel;

		OutputStreamInterface * m_stream;
	};
}
