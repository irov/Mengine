#include "SDL3FileOutputStream.h"

#include "Interface/FileSystemInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/PathHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/DebugFileHelper.h"

#include "Config/Path.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDL3FileOutputStream::SDL3FileOutputStream()
        : m_IOStream( nullptr )
        , m_size( 0 )
        , m_withTemp( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDL3FileOutputStream::~SDL3FileOutputStream()
    {
        this->close();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3FileOutputStream::open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, bool _withTemp )
    {
        m_relationPath = _relationPath;
        m_folderPath = _folderPath;
        m_filePath = _filePath;

        m_withTemp = _withTemp;

        Path concatenatePath = {'\0'};

#if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_PLATFORM_UWP)
        if( m_withTemp == true )
        {
            if( Helper::concatenateFilePath( {m_relationPath, m_folderPath, m_filePath, STRINGIZE_FILEPATH_LOCAL( ".~tmp" )}, concatenatePath ) == false )
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
            if( Helper::concatenateFilePath( {m_relationPath, m_folderPath, m_filePath}, concatenatePath ) == false )
            {
                LOGGER_ERROR( "invalid concatenate filePath '%s:%s'"
                    , m_folderPath.c_str()
                    , m_filePath.c_str()
                );

                return false;
            }
        }
#else
        if( Helper::concatenateFilePath( {m_relationPath, m_folderPath, m_filePath}, concatenatePath ) == false )
        {
            LOGGER_ERROR( "invalid concatenate filePath '%s:%s'"
                , m_folderPath.c_str()
                , m_filePath.c_str()
            );

            return false;
        }
#endif

        SDL_IOStream * IOStream = SDL_IOFromFile( concatenatePath, "wb" );

        if( IOStream == nullptr )
        {
            LOGGER_ERROR( "file %s invalid open error '%s'"
                , concatenatePath
                , SDL_GetError()
            );

            return false;
        }

        m_IOStream = IOStream;

        m_size = 0;

#if defined(MENGINE_DEBUG)
        if( SERVICE_IS_INITIALIZE( NotificationServiceInterface ) == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_DEBUG_OPEN_FILE, _folderPath, _filePath, false, false );
        }
#endif

#if defined(MENGINE_DEBUG_FILE_PATH_ENABLE)
        Helper::addDebugFilePath( this, _relationPath, _folderPath, _filePath, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3FileOutputStream::close()
    {
        if( m_IOStream == nullptr )
        {
            return true;
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

#if defined(MENGINE_DEBUG_FILE_PATH_ENABLE)
        Helper::removeDebugFilePath( this );
#endif

        bool result = SDL_CloseIO( m_IOStream );
        m_IOStream = nullptr;

        m_size = 0;

        if( result == false )
        {
            LOGGER_ERROR( "file '%s:%s' invalid close error '%s'"
                , m_folderPath.c_str()
                , m_filePath.c_str()
                , SDL_GetError()
            );

            return false;
        }

#if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_PLATFORM_UWP)
        if( m_withTemp == true )
        {
            Path fullPathTemp = {'\0'};
            if( Helper::concatenateFilePath( {m_relationPath, m_folderPath, m_filePath, STRINGIZE_FILEPATH_LOCAL( ".~tmp" )}, fullPathTemp ) == false )
            {
                LOGGER_ERROR( "invalid concatenate filePath '%s:%s'"
                    , m_folderPath.c_str()
                    , m_filePath.c_str()
                );

                return false;
            }

            Path fullPath = {'\0'};
            if( Helper::concatenateFilePath( {m_relationPath, m_folderPath, m_filePath}, fullPath ) == false )
            {
                LOGGER_ERROR( "invalid concatenate filePath '%s:%s'"
                    , m_folderPath.c_str()
                    , m_filePath.c_str()
                );

                return false;
            }

            if( FILE_SYSTEM()
                ->moveFile( fullPathTemp, fullPath ) == false )
            {
                LOGGER_ERROR( "invalid move file from '%s' to '%s'"
                    , fullPathTemp
                    , fullPath
                );

                return false;
            }
        }
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDL3FileOutputStream::write( const void * _data, size_t _size )
    {
        size_t written = SDL_WriteIO( m_IOStream, _data, _size );

        if( written != _size )
        {
            LOGGER_ERROR( "invalid write file '%s:%s' size %zu [error: %s]"
                , m_folderPath.c_str()
                , m_filePath.c_str()
                , _size
                , SDL_GetError()
            );

            return 0;
        }

        m_size += written;

        return written;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDL3FileOutputStream::size() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3FileOutputStream::flush()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    SDL_IOStream * SDL3FileOutputStream::getIOStream() const
    {
        return m_IOStream;
    }
    //////////////////////////////////////////////////////////////////////////
}