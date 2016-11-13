#	include "SDLFileGroupDirectory.h"

#   include "Interface/SDLLayerInterface.h"

#	include "Logger/Logger.h"

#	include "Core/String.h"

#   include "SDL_rwops.h"


namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    SDLFileGroupDirectory::SDLFileGroupDirectory()
        : m_serviceProvider(nullptr)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLFileGroupDirectory::~SDLFileGroupDirectory()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLFileGroupDirectory::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * SDLFileGroupDirectory::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileGroupDirectory::initialize( const FilePath & _path )
    {
        m_path = _path;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLFileGroupDirectory::finalize()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileGroupDirectory::isPacked() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & SDLFileGroupDirectory::getPath() const
    {
        return m_path;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileGroupDirectory::existFile( const FilePath & _fileName ) const
    {
        // SDL doesn't have this, so we're emulating ... ugly way :(

        Char filePath[MENGINE_MAX_PATH];
        if( SDLLAYER_SERVICE(m_serviceProvider)
            ->concatenateFilePath( m_path, _fileName, filePath, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("SDLFileInputStream::open invlalid concatenate filePath '%s':'%s'"
                , m_path.c_str()
                , _fileName.c_str()
                );

            return false;
        }

        bool exist = false;
        SDL_RWops* file = SDL_RWFromFile(filePath, "rb");
        if( file != nullptr )
        {
            SDL_RWclose(file);
            exist = true;
        }

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr SDLFileGroupDirectory::createInputFile( const FilePath & _fileName, bool _streaming )
    {
        (void)_fileName;
        (void)_streaming;

        SDLFileInputStream * inputStream = m_factoryInputStream.createObject();

        inputStream->setServiceProvider( m_serviceProvider );

        return inputStream;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileGroupDirectory::openInputFile( const FilePath & _fileName, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, bool _streaming )
    {
        (void)_streaming;

        if( _stream == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("SDLFileGroupDirectory::openInputFile failed _stream == NULL"
                );

            return false;
        }

        FileInputStreamInterface * file = stdex::intrusive_get<FileInputStreamInterface *>(_stream);

        if( file->open( m_path, _fileName, _offset, _size ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("SDLFileGroupDirectory::openInputFile failed open file '%s':'%s'"
                , m_path.c_str()
                , _fileName.c_str()
                );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    OutputStreamInterfacePtr SDLFileGroupDirectory::createOutputFile()
    {
        SDLFileOutputStream * outputStream = m_factoryOutputStream.createObject();

        outputStream->setServiceProvider( m_serviceProvider );

        return outputStream;
    }
    //////////////////////////////////////////////////////////////////////////	
    bool SDLFileGroupDirectory::openOutputFile( const FilePath & _fileName, const OutputStreamInterfacePtr & _stream )
    {
        if( _stream == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("SDLFileGroupDirectory::openOutputFile failed _stream == NULL"
                );

            return false;
        }

        FileOutputStreamInterface * file = stdex::intrusive_get<FileOutputStreamInterface *>(_stream);

        if( file->open( m_path, _fileName ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("SDLFileGroupDirectory::openOutputFile failed open file '%s':'%s'"
                , m_path.c_str()
                , _fileName.c_str()
                );

            return false;
        }
                
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    FileMappedInterfacePtr SDLFileGroupDirectory::createMappedFile()
    {
        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileGroupDirectory::openMappedFile( const FilePath & _fileName, const FileMappedInterfacePtr & _stream )
    {
        (void)_fileName;
        (void)_stream;
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileGroupDirectory::existDirectory( const FilePath & _path ) const
    {
        (void)_path;
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileGroupDirectory::createDirectory( const FilePath & _path )
    {
        (void)_path;
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileGroupDirectory::removeDirectory( const FilePath & _path )
    {
        (void)_path;
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileGroupDirectory::removeFile( const FilePath & _fileName )
    {
        (void)_fileName;
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}   // namespace Menge
