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
		virtual ~FileLogger();

	public:
		void setVerboseLevel( EMessageLevel _level ) override;
        void setVerboseFlag( uint32_t _flag ) override;

    public:
		bool validMessage( EMessageLevel _level, uint32_t _flag ) const override;

	public:
		OutputStreamInterfacePtr getStream() const;

	protected:
		void log( EMessageLevel _level, uint32_t _flag, const char * _data, size_t _count ) override;

    protected:
		void flush() override;

	protected:
		EMessageLevel m_verboseLevel;
        uint32_t m_verboseFlag;

		OutputStreamInterfacePtr m_stream;
	};
}
