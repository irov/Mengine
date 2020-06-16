#include "SDLFileOutputStream.h"

#include "Kernel/Logger.h"

#include "SDLFileHelper.h"

#include "SDL_rwops.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLFileOutputStream::SDLFileOutputStream()
        : m_rwops( nullptr )
        , m_size( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLFileOutputStream::~SDLFileOutputStream()
    {
        if( m_rwops != nullptr )
        {
            SDL_RWclose( m_rwops );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileOutputStream::open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath )
    {
        Char concatenatePath[MENGINE_MAX_PATH];
        if( Helper::concatenateFilePath( _relationPath, _folderPath, _filePath, concatenatePath, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR( "invalid concatenate filePath '%s':'%s'"
                , _folderPath.c_str()
                , _filePath.c_str()
            );

            return false;
        }

        m_rwops = SDL_RWFromFile( concatenatePath, "wb" );

        if( m_rwops == nullptr )
        {
            const char * sdl_error = SDL_GetError();

            LOGGER_ERROR( "%s invalid open error '%s'"
                , concatenatePath
                , sdl_error
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDLFileOutputStream::write( const void * _data, size_t _size )
    {
        const size_t written = SDL_RWwrite( m_rwops, _data, 1, _size );

        if( written != _size )
        {
            const char * sdl_error = SDL_GetError();

            LOGGER_ERROR( "invalid write %zu [written %zu] error '%s'"
                , _size
                , written
                , sdl_error
            );

            return 0;
        }

        m_size += written;

        return written;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDLFileOutputStream::size() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileOutputStream::flush()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
