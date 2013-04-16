#	include "FileLogger.h"
#	include "Interface/FileSystemInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileLogger::FileLogger( OutputStreamInterface* _fileStream )
		: m_stream(_fileStream)
		, m_verboseLevel(LM_INFO)
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	void FileLogger::setVerboseLevel( EMessageLevel _level )
	{
		m_verboseLevel = _level;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileLogger::validVerboseLevel( EMessageLevel _level ) const
	{
		if( m_verboseLevel < _level )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileLogger::log( const char * _data, int _count, EMessageLevel _level )
	{
        (void)_level;

		m_stream->write( _data, _count );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileLogger::flush()
	{
		m_stream->flush();
	}
	//////////////////////////////////////////////////////////////////////////
	OutputStreamInterface* FileLogger::getStream() const
	{
		return m_stream;
	}
}

