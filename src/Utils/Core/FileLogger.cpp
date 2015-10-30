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
		: m_serviceProvider(nullptr)
		, m_verboseLevel(LM_INFO)
        , m_verboseFlag(0xFFFFFFFF)
	{		
	}
	//////////////////////////////////////////////////////////////////////////
	FileLogger::~FileLogger()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void FileLogger::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * FileLogger::getServiceProvider() const
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileLogger::initialize()
	{
		WString date;
		Helper::makeDateTime( date );

		WString unicode_logFilename;
		unicode_logFilename += L"Game";

		bool developmentMode = HAS_OPTIONS( m_serviceProvider, "dev" );
		bool roamingMode = HAS_OPTIONS( m_serviceProvider, "roaming" );

		if( developmentMode == true && roamingMode == false )
		{
			unicode_logFilename += L"_";
			unicode_logFilename += date;
		}

		unicode_logFilename += L".log";

		String utf8_logFilename;
		if( Helper::unicodeToUtf8( m_serviceProvider, unicode_logFilename, utf8_logFilename ) == false )
		{
			return false;
		}

		FilePath logFilename = Helper::stringizeString( m_serviceProvider, utf8_logFilename );

		m_stream = FILE_SERVICE( m_serviceProvider)
			->openOutputFile( STRINGIZE_STRING_LOCAL( m_serviceProvider, "user" ), logFilename );
		
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

