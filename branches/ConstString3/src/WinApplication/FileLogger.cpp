#	include "FileLogger.h"
#	include "Interface/FileSystemInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileLogger::FileLogger()
		: m_fileOutIterface(NULL)
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
		m_fileOutIterface->write( _data, _count );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileLogger::flush()
	{
		m_fileOutIterface->flush();
	}
	//////////////////////////////////////////////////////////////////////////
	FileOutputStreamInterface* FileLogger::getFileInterface()
	{
		return m_fileOutIterface;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileLogger::setFileInterface( FileOutputStreamInterface* _fileInterface )
	{
		m_fileOutIterface = _fileInterface;
	}
}

