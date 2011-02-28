#	include "FileLogger.h"
#	include "Interface/FileSystemInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileLogger::FileLogger()
		: m_fileOutIterface(NULL)
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	void FileLogger::log( const void* _data, int _count, EMessageLevel _level )
	{
		m_fileOutIterface->write( _data, _count );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileLogger::flush()
	{
		m_fileOutIterface->flush();
	}
	//////////////////////////////////////////////////////////////////////////
	FileOutputInterface* FileLogger::getFileInterface()
	{
		return m_fileOutIterface;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileLogger::setFileInterface( FileOutputInterface* _fileInterface )
	{
		m_fileOutIterface = _fileInterface;
	}
}

