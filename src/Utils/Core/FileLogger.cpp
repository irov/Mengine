#	include "FileLogger.h"

#	include "Interface/ApplicationInterface.h"
#	include "Interface/OptionsInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/UnicodeInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Core/Date.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileLogger::FileLogger()
		: m_verboseLevel(LM_INFO)
        , m_verboseFlag(0xFFFFFFFF)
	{		
	}
	//////////////////////////////////////////////////////////////////////////
	FileLogger::~FileLogger()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileLogger::initialize()
	{
		String utf8_logFilename;
		utf8_logFilename += "Game";

		utf8_logFilename += "_";
        
        String date;
        Helper::makeDateTime( date );
		utf8_logFilename += date;

		utf8_logFilename += ".log";

		FilePath logFilename = Helper::stringizeFilePath( utf8_logFilename.c_str(), utf8_logFilename.size() );

		m_stream = FILE_SERVICE()
			->openOutputFile( STRINGIZE_STRING_LOCAL( "user" ), logFilename );

        if( m_stream == nullptr )
        {
            return false;
        }
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileLogger::finalize()
	{
		if( m_stream != nullptr )
		{
			m_stream->flush();
			m_stream = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void FileLogger::setVerboseLevel( EMessageLevel _level )
	{
		m_verboseLevel = _level;
	}
    //////////////////////////////////////////////////////////////////////////
    void FileLogger::setVerboseFlag( uint32_t _flag )
    {
        m_verboseFlag = _flag;
    }
	//////////////////////////////////////////////////////////////////////////
	bool FileLogger::validMessage( EMessageLevel _level, uint32_t _flag ) const
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
	void FileLogger::log( EMessageLevel _level, uint32_t _flag, const char * _data, size_t _count )
	{
        (void)_level;
        (void)_flag;

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

