#	include "SDLFileGroupDirectory.h"

#   include "Interface/SDLLayerInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/PlatformInterface.h"

#	include "SDLFileInputStream.h"
#	include "SDLFileOutputStream.h"
#	include "SDLFileMapped.h"

#   include "Factory/FactoryPool.h"

#	include "Logger/Logger.h"

#	include "Core/String.h"

#   include "SDL_rwops.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    SDLFileGroupDirectory::SDLFileGroupDirectory()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLFileGroupDirectory::~SDLFileGroupDirectory()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileGroupDirectory::initialize( const ConstString & _name, const ConstString & _category, const FilePath & _folderPath )
    {
		m_name = _name;
		m_category = _category;
        m_folderPath = _folderPath;

        m_factoryInputStream = new FactoryPool<SDLFileInputStream, 8>( m_serviceProvider );
        m_factoryOutputStream = new FactoryPool<SDLFileOutputStream, 4>( m_serviceProvider );

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
    const FilePath & SDLFileGroupDirectory::getRelationPath() const
    {
        return m_relationPath;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & SDLFileGroupDirectory::getFolderPath() const
    {
        return m_folderPath;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileGroupDirectory::existFile( const FilePath & _fileName ) const
    {
        // SDL doesn't have this, so we're emulating ... ugly way :(

        Char filePath[MENGINE_MAX_PATH];
        if( SDLLAYER_SERVICE(m_serviceProvider)
            ->concatenateFilePath( m_relationPath, m_folderPath, _fileName, filePath, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("SDLFileInputStream::open invlalid concatenate filePath '%s':'%s'"
                , m_folderPath.c_str()
                , _fileName.c_str()
                );

            return false;
        }

        SDL_RWops* rwops = SDL_RWFromFile( filePath, "rb" );

        if( rwops != nullptr )
        {
            Sint64 size = SDL_RWsize(rwops);
            (void)size;
            
            SDL_RWclose( rwops );
            
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileGroupDirectory::existDirectory( const FilePath & _folderName ) const
    {
        const FilePath & relationPath = this->getRelationPath();
        const FilePath & folderPath = this->getFolderPath();

        PathString accountString;
        accountString.append( relationPath );
        accountString.append( folderPath );
        accountString.append( _folderName );
        accountString.append( '/' );

        WString unicode_folderPath;
        if( Helper::utf8ToUnicode( m_serviceProvider, accountString, unicode_folderPath ) == false )
        {
            return false;
        }

        if( PLATFORM_SERVICE( m_serviceProvider )
            ->existDirectory( unicode_folderPath ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileGroupDirectory::createDirectory( const FilePath & _folderName ) const
    {
        const FilePath & relationPath = this->getRelationPath();
        const FilePath & folderPath = this->getFolderPath();

        PathString accountString;
        accountString.append( relationPath );
        accountString.append( folderPath );
        accountString.append( _folderName );

        WString unicode_folderPath;
        if( Helper::utf8ToUnicode( m_serviceProvider, accountString, unicode_folderPath ) == false )
        {
            return false;
        }

        if( PLATFORM_SERVICE( m_serviceProvider )
            ->existDirectory( unicode_folderPath ) == true )
        {
            return true;
        }

        if( PLATFORM_SERVICE( m_serviceProvider )
            ->createDirectory( unicode_folderPath ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr SDLFileGroupDirectory::createInputFile( const FilePath & _fileName, bool _streaming )
    {
        (void)_fileName;
        (void)_streaming;

        SDLFileInputStreamPtr inputStream = m_factoryInputStream->createObject();

        inputStream->setServiceProvider( m_serviceProvider );

        return inputStream;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileGroupDirectory::openInputFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, bool _streaming )
    {
        (void)_streaming;

        if( _stream == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("SDLFileGroupDirectory::openInputFile failed _stream == NULL"
                );

            return false;
        }

        FileInputStreamInterface * file = stdex::intrusive_get<FileInputStreamInterface *>(_stream);

        if( file->open( m_relationPath, m_folderPath, _filePath, _offset, _size ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("SDLFileGroupDirectory::openInputFile failed open file '%s':'%s'"
                , m_folderPath.c_str()
                , _filePath.c_str()
                );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    OutputStreamInterfacePtr SDLFileGroupDirectory::createOutputFile()
    {
        SDLFileOutputStreamPtr outputStream = m_factoryOutputStream->createObject();

        outputStream->setServiceProvider( m_serviceProvider );

        return outputStream;
    }
    //////////////////////////////////////////////////////////////////////////	
    bool SDLFileGroupDirectory::openOutputFile( const FilePath & _filePath, const OutputStreamInterfacePtr & _stream )
    {
        if( _stream == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("SDLFileGroupDirectory::openOutputFile failed _stream == NULL"
                );

            return false;
        }

        FileOutputStreamInterface * file = stdex::intrusive_get<FileOutputStreamInterface *>(_stream);

        if( file->open( m_relationPath, m_folderPath, _filePath ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("SDLFileGroupDirectory::openOutputFile failed open file '%s':'%s'"
                , m_folderPath.c_str()
                , _filePath.c_str()
                );

            return false;
        }
                
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLFileGroupDirectory::setRelationPath( const FilePath & _relationPath )
    {
        m_relationPath = _relationPath;
    }
    //////////////////////////////////////////////////////////////////////////
}   // namespace Menge
