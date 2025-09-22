#include "AndroidFileOutputStream.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/FileSystemInterface.h"

#include "Environment/Android/AndroidAssetServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/DebugFileHelper.h"
#include "Kernel/ThreadGuardScope.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/FilePathHelper.h"

#include "Config/Path.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidFileOutputStream::AndroidFileOutputStream()
        : m_file( nullptr )
        , m_size( 0 )
        , m_withTemp( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidFileOutputStream::~AndroidFileOutputStream()
    {
        this->close();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidFileOutputStream::open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, bool _withTemp )
    {
        MENGINE_THREAD_GUARD_SCOPE( AndroidFileOutputStream, this );

        m_relationPath = _relationPath;
        m_folderPath = _folderPath;
        m_filePath = _filePath;

        m_withTemp = _withTemp;

        Path fullPath = {'\0'};
        if( m_withTemp == true )
        {
            if( Helper::concatenateFilePath( {m_relationPath, m_folderPath, m_filePath, STRINGIZE_FILEPATH_LOCAL( ".~tmp" )}, fullPath ) == false )
            {
                LOGGER_ERROR( "invalid concatenate filePath '%s:%s' [temp]"
                    , m_folderPath.c_str()
                    , m_filePath.c_str()
                );

                return false;
            }
        }
        else
        {
            if( Helper::concatenateFilePath( {m_relationPath, m_folderPath, m_filePath}, fullPath ) == false )
            {
                LOGGER_ERROR( "invalid concatenate filePath '%s:%s'"
                    , m_folderPath.c_str()
                    , m_filePath.c_str()
                );

                return false;
            }
        }

        FILE * file = ::fopen( fullPath, "wb" );

        if( file == nullptr )
        {
            LOGGER_ERROR( "invalid open file '%s'"
                , fullPath
            );

            return false;
        }

        m_file = file;

        m_size = 0;

#if defined(MENGINE_DEBUG)
        if( SERVICE_IS_INITIALIZE( NotificationServiceInterface ) == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_DEBUG_OPEN_FILE, m_folderPath, m_filePath, false, false );
        }
#endif

#if defined(MENGINE_DEBUG_FILE_PATH_ENABLE)
        Helper::addDebugFilePath( this, m_relationPath, m_folderPath, m_filePath, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidFileOutputStream::close()
    {
        MENGINE_THREAD_GUARD_SCOPE( AndroidFileOutputStream, this );

        if( m_file == nullptr )
        {
            return;
        }

        MENGINE_ASSERTION_FATAL( m_size != 0, "file '%s:%s' is empty"
            , m_folderPath.c_str()
            , m_filePath.c_str()
        );

#if defined(MENGINE_DEBUG)
        if( SERVICE_IS_INITIALIZE( NotificationServiceInterface ) == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_DEBUG_CLOSE_FILE, m_folderPath, m_filePath, false, false );
        }
#endif

        ::fclose( m_file );
        m_file = nullptr;

        m_size = 0;

        if( m_withTemp == true )
        {
            Path fullPathTemp = {'\0'};
            Helper::concatenateFilePath( {m_relationPath, m_folderPath, m_filePath, STRINGIZE_FILEPATH_LOCAL( ".~tmp" )}, fullPathTemp );

            Path fullPath = {'\0'};
            Helper::concatenateFilePath( {m_relationPath, m_folderPath, m_filePath}, fullPath );

            if( FILE_SYSTEM()
                ->moveFile( fullPathTemp, fullPath ) == false )
            {
                LOGGER_ERROR( "invalid move close file from '%s' to '%s'"
                    , fullPathTemp
                    , fullPath
                );
            }
        }

#if defined(MENGINE_DEBUG_FILE_PATH_ENABLE)
        Helper::removeDebugFilePath( this );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidFileOutputStream::write( const void * _data, size_t _size )
    {
        size_t written = ::fwrite( _data, 1, _size, m_file );

        if( written != _size )
        {
            LOGGER_ERROR( "invalid write file '%s:%s' size: %zu error: %d"
                , m_folderPath.c_str()
                , m_filePath.c_str()
                , _size
                , errno
            );

            return 0;
        }

        m_size += written;

        return written;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidFileOutputStream::size() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidFileOutputStream::flush()
    {
        ::fflush( m_file );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
