#include "SDL2FileOutputStream.h"

#include "Interface/FileSystemInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/PathHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/NotificationHelper.h"

#include "Config/Path.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDL2FileOutputStream::SDL2FileOutputStream()
        : m_rwops( nullptr )
        , m_size( 0 )
        , m_withTemp( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDL2FileOutputStream::~SDL2FileOutputStream()
    {
        this->close();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2FileOutputStream::open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, bool _withTemp )
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

        SDL_RWops * rwops = SDL_RWFromFile( concatenatePath, "wb" );

        if( rwops == nullptr )
        {
            LOGGER_ERROR( "file %s invalid open error '%s'"
                , concatenatePath
                , SDL_GetError()
            );

            return false;
        }

        m_rwops = rwops;

        m_size = 0;

#if defined(MENGINE_DEBUG)
        if( SERVICE_IS_INITIALIZE( NotificationServiceInterface ) == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_DEBUG_OPEN_FILE, _folderPath, _filePath, false, false );
        }
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2FileOutputStream::close()
    {
        if( m_rwops == nullptr )
        {
            return true;
        }

#if defined(MENGINE_DEBUG)
        if( SERVICE_IS_INITIALIZE( NotificationServiceInterface ) == true )
        {
            const FilePath & folderPath = this->getDebugFolderPath();
            const FilePath & filePath = this->getDebugFilePath();

            NOTIFICATION_NOTIFY( NOTIFICATOR_DEBUG_CLOSE_FILE, folderPath, filePath, false, false );
        }
#endif

        SDL_RWclose( m_rwops );
        m_rwops = nullptr;

        m_size = 0;

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
                return false;
            }
        }
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDL2FileOutputStream::write( const void * _data, size_t _size )
    {
        size_t written = SDL_RWwrite( m_rwops, _data, 1, _size );

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
    size_t SDL2FileOutputStream::size() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2FileOutputStream::flush()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    SDL_RWops * SDL2FileOutputStream::getRWops() const
    {
        return m_rwops;
    }
    //////////////////////////////////////////////////////////////////////////
}
