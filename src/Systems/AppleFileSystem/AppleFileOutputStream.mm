#include "AppleFileOutputStream.h"

#include "Interface/PlatformServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/PathHelper.h"
#include "Kernel/FilePathHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleFileOutputStream::AppleFileOutputStream()
        : m_rwops( nullptr )
        , m_size( 0 )
        , m_withTemp( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleFileOutputStream::~AppleFileOutputStream()
    {
        this->close();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFileOutputStream::open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, bool _withTemp )
    {
        m_relationPath = _relationPath;
        m_folderPath = _folderPath;
        m_filePath = _filePath;

        m_withTemp = _withTemp;

        Char concatenatePath[MENGINE_MAX_PATH + 1] = {'\0'};

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

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFileOutputStream::close()
    {
        if( m_rwops == nullptr )
        {
            return true;
        }

        SDL_RWclose( m_rwops );
        m_rwops = nullptr;

        m_size = 0;

#if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_PLATFORM_UWP)
        if( m_withTemp == true )
        {
            Char fullPathTemp[MENGINE_MAX_PATH + 1] = {'\0'};
            if( Helper::concatenateFilePath( {m_relationPath, m_folderPath, m_filePath, STRINGIZE_FILEPATH_LOCAL( ".~tmp" )}, fullPathTemp ) == false )
            {
                LOGGER_ERROR( "invalid concatenate filePath '%s:%s'"
                    , m_folderPath.c_str()
                    , m_filePath.c_str()
                );

                return false;
            }

            Char fullPath[MENGINE_MAX_PATH + 1] = {'\0'};
            if( Helper::concatenateFilePath( {m_relationPath, m_folderPath, m_filePath}, fullPath ) == false )
            {
                LOGGER_ERROR( "invalid concatenate filePath '%s:%s'"
                    , m_folderPath.c_str()
                    , m_filePath.c_str()
                );

                return false;
            }

            if( PLATFORM_SERVICE()
                ->moveFile( fullPathTemp, fullPath ) == false )
            {
                return false;
            }
        }
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AppleFileOutputStream::write( const void * _data, size_t _size )
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
    size_t AppleFileOutputStream::size() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFileOutputStream::flush()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    SDL_RWops * AppleFileOutputStream::getRWops() const
    {
        return m_rwops;
    }
    //////////////////////////////////////////////////////////////////////////
}
