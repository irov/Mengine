#include "FileLogger.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/UnicodeSystemInterface.h"

#include "Kernel/Document.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"

#include "Config/Stringstream.h"

#include <iomanip>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FileLogger::FileLogger()
        : m_verboseLevel( LM_INFO )
        , m_verboseFlag( 0xFFFFFFFF )
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

        PlatformDateTime dateTime;
        PLATFORM_SERVICE()
            ->getDateTime( &dateTime );

        Stringstream ss_date;
        ss_date << dateTime.year
            << "_" << std::setw( 2 ) << std::setfill( '0' ) << (dateTime.month)
            << "_" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.day
            << "_" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.hour
            << "_" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.minute
            << "_" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.second;

        String str_date = ss_date.str();
        
        utf8_logFilename += str_date;

        utf8_logFilename += ".log";

        FilePath logFilename = Helper::stringizeFilePath( utf8_logFilename );

        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

        m_stream = FILE_SERVICE()
            ->openOutputFile( fileGroup, logFilename, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( m_stream, false );

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
    void FileLogger::log( EMessageLevel _level, uint32_t _flag, const Char * _data, uint32_t _count )
    {
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _flag );

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

