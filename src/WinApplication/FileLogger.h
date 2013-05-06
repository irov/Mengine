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
        void setVerboseFlag( size_t _flag ) override;

    public:
		bool validMessage( EMessageLevel _level, size_t _flag ) const override;

	public:
		OutputStreamInterfacePtr getStream() const;

	protected:
		void log( EMessageLevel _level, size_t _flag, const char * _data, size_t _count ) override;

    protected:
		void flush() override;

	protected:
		EMessageLevel m_verboseLevel;
        size_t m_verboseFlag;

		OutputStreamInterfacePtr m_stream;
	};
}
