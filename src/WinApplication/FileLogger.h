#	pragma once

#	include "Interface/StreamInterface.h"
#	include "Interface/LogSystemInterface.h"

namespace Menge
{
	class FileLogger
		: public LoggerInterface
	{
	public:
		FileLogger( const OutputStreamInterfacePtr & _fileStream );

	public:
		void setVerboseLevel( EMessageLevel _level ) override;
		bool validVerboseLevel( EMessageLevel _level ) const override;

	public:
		OutputStreamInterfacePtr getStream() const;

	protected:
		void log( const char * _data, size_t _count, EMessageLevel _level ) override;
		void flush() override;

	protected:
		EMessageLevel m_verboseLevel;

		OutputStreamInterfacePtr m_stream;
	};
}
