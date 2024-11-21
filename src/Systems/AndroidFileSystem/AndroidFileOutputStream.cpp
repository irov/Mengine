#include "AndroidFileOutputStream.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/AndroidAssetServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/DebugFileHelper.h"
#include "Kernel/ThreadGuardScope.h"
#include "Kernel/NotificationHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidFileOutputStream::AndroidFileOutputStream()
        : m_file( nullptr )
        , m_size( 0 )
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
        MENGINE_UNUSED( _withTemp );

        MENGINE_THREAD_GUARD_SCOPE( AndroidFileOutputStream, this );

#if defined(MENGINE_DEBUG)
        this->setDebugRelationPath( _relationPath );
        this->setDebugFolderPath( _folderPath );
        this->setDebugFilePath( _filePath );
#endif

        Path fullPath = {'\0'};
        if( Helper::concatenateFilePath( {_relationPath, _folderPath, _filePath}, fullPath ) == false )
        {
            LOGGER_ERROR( "invalid concatenate filePath '%s:%s'"
                , _folderPath.c_str()
                , _filePath.c_str()
            );

            return false;
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
            const FilePath & folderPath = this->getDebugFolderPath();
            const FilePath & filePath = this->getDebugFilePath();

            NOTIFICATION_NOTIFY( NOTIFICATOR_DEBUG_OPEN_FILE, folderPath, filePath, false, false );
        }
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidFileOutputStream::close()
    {
        MENGINE_THREAD_GUARD_SCOPE( AndroidFileOutputStream, this );

        if( m_file == nullptr )
        {
            return false;
        }

#if defined(MENGINE_DEBUG)
        if( SERVICE_IS_INITIALIZE( NotificationServiceInterface ) == true )
        {
            const FilePath & folderPath = this->getDebugFolderPath();
            const FilePath & filePath = this->getDebugFilePath();

            NOTIFICATION_NOTIFY( NOTIFICATOR_DEBUG_CLOSE_FILE, folderPath, filePath, false, false );
        }
#endif

        ::fclose( m_file );
        m_file = nullptr;

        m_size = 0;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidFileOutputStream::write( const void * _data, size_t _size )
    {
        size_t written = ::fwrite( _data, 1, _size, m_file );

        if( written != _size )
        {
            LOGGER_ERROR( "invalid write file '%s' size: %zu error: %d"
                , Helper::getDebugFullPath( this ).c_str()
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
