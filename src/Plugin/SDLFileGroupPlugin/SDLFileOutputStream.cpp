#include "SDLFileOutputStream.h"

#include "Interface/UnicodeInterface.h"
#include "Interface/SDLLayerInterface.h"

#include "Kernel/Logger.h"

#include "SDL_rwops.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLFileOutputStream::SDLFileOutputStream()
        : m_rwops(nullptr)
        , m_size(0)
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
        if( SDLLAYER_SERVICE()
            ->concatenateFilePath( _relationPath, _folderPath, _filePath, concatenatePath, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR("SDLFileOutputStream::open invlalid concatenate filePath '%s':'%s'"
                , _folderPath.c_str()
                , _filePath.c_str()
                );

            return false;
        }

        m_rwops = SDL_RWFromFile(concatenatePath, "wb");

        if ( m_rwops == nullptr )
        {
            LOGGER_ERROR("SDLFileOutputStream::open %s invalid open"
                , concatenatePath
                );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileOutputStream::write( const void * _data, size_t _size )
    {
        const size_t written = SDL_RWwrite(m_rwops, _data, 1, _size);

        if( written != _size )
        {
            LOGGER_ERROR("SDLFileOutputStream::write invalid %d"
                , _size
                );

            return false;
        }

        m_size += written;

        return true;
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
}	// namespace Mengine
