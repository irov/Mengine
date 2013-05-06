#	include "FileLogger.h"
#	include "Interface/FileSystemInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileLogger::FileLogger( const OutputStreamInterfacePtr & _fileStream )
		: m_stream(_fileStream)
		, m_verboseLevel(LM_INFO)
        , m_verboseFlag(0xFFFFFFFF)
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	void FileLogger::setVerboseLevel( EMessageLevel _level )
	{
		m_verboseLevel = _level;
	}
    //////////////////////////////////////////////////////////////////////////
    void FileLogger::setVerboseFlag( size_t _flag )
    {
        m_verboseFlag = _flag;
    }
	//////////////////////////////////////////////////////////////////////////
	bool FileLogger::validMessage( EMessageLevel _level, size_t _flag ) const
	{
		if( m_verboseLevel < _level )
		{
			return false;
		}

        if( _flag == 0 )
        {
            return true;
        }

        if( (m_verboseFlag & _flag) == 0 )
        {
            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileLogger::log( EMessageLevel _level, size_t _flag, const char * _data, size_t _count )
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
	OutputStreamInterfacePtr FileLogger::getStream() const
	{
		return m_stream;
	}
}

