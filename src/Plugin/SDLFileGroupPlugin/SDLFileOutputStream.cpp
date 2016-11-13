#	include "SDLFileOutputStream.h"

#	include "Interface/UnicodeInterface.h"
#   include "Interface/SDLLayerInterface.h"

#   include "Utils/Logger/Logger.h"

#   include "SDL_rwops.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    SDLFileOutputStream::SDLFileOutputStream()
        : m_serviceProvider(nullptr)
        , m_rwops(nullptr)
        , m_size(0)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLFileOutputStream::~SDLFileOutputStream()
    {
        if( m_rwops != nullptr )
        {
            SDL_RWclose(m_rwops);
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLFileOutputStream::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileOutputStream::open( const FilePath & _folder, const FilePath& _fileName )
    {
        Char filePath[MENGINE_MAX_PATH];
        if( SDLLAYER_SERVICE(m_serviceProvider)
            ->concatenateFilePath( _folder, _fileName, filePath, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("SDLFileOutputStream::open invlalid concatenate filePath '%s':'%s'"
                , _folder.c_str()
                , _fileName.c_str()
                );

            return false;
        }

        m_rwops = SDL_RWFromFile(filePath, "wb");

        if ( m_rwops == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("SDLFileOutputStream::open %s invalid open"
                , filePath
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
            LOGGER_ERROR(m_serviceProvider)("SDLFileOutputStream::write invalid %d"
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
}	// namespace Menge
