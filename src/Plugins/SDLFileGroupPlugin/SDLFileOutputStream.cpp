#include "SDLFileOutputStream.h"

#include "Interface/PlatformInterface.h"

#include "Kernel/Logger.h"

#include "SDLFileHelper.h"

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
        this->close();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileOutputStream::open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath )
    {
        m_relationPath = _relationPath;
        m_folderPath = _folderPath;
        m_filePath = _filePath;

        Char concatenatePath[MENGINE_MAX_PATH] = {'\0'};
        if( Helper::concatenateFilePathTemp( m_relationPath, m_folderPath, m_filePath, concatenatePath, MENGINE_MAX_PATH - 1 ) == false )
        {
            LOGGER_ERROR( "invalid concatenate filePath '%s':'%s'"
                , m_folderPath.c_str()
                , m_filePath.c_str()
            );

            return false;
        }

        SDL_RWops * rwops = SDL_RWFromFile( concatenatePath, "wb" );

        if( rwops == nullptr )
        {
            LOGGER_ERROR( "%s invalid open error '%s'"
                , concatenatePath
                , SDL_GetError()
            );

            return false;
        }

        m_rwops = rwops;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileOutputStream::close()
    {
        if( m_rwops == nullptr )
        {
            return true;
        }

        bool successful = this->flush();

        SDL_RWclose( m_rwops );
        m_rwops = nullptr;

        Char fullPathTemp[MENGINE_MAX_PATH] = {'\0'};
        if( Helper::concatenateFilePathTemp( m_relationPath, m_folderPath, m_filePath, fullPathTemp, MENGINE_MAX_PATH - 1 ) == false )
        {
            LOGGER_ERROR( "invalid concatenate filePath '%s':'%s'"
                , m_folderPath.c_str()
                , m_filePath.c_str()
            );

            return false;
        }

        Char fullPath[MENGINE_MAX_PATH] = {'\0'};
        if( Helper::concatenateFilePath( m_relationPath, m_folderPath, m_filePath, fullPath, MENGINE_MAX_PATH - 1 ) == false )
        {
            LOGGER_ERROR( "invalid concatenate filePath '%s':'%s'"
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

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDLFileOutputStream::write( const void * _data, size_t _size )
    {
        size_t written = SDL_RWwrite( m_rwops, _data, 1, _size );

        if( written != _size )
        {
            LOGGER_ERROR( "invalid write file '%s' size %zu [error: %s]"
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
