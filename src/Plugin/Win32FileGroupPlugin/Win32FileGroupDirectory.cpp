#include "Win32FileGroupDirectory.h"

#include "Interface/MemoryInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/UnicodeSystemInterface.h"

#include "Win32FileHelper.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/FactoryAssertion.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/String.h"

#include "Environment/WIN32/WindowsIncluder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32FileGroupDirectory::Win32FileGroupDirectory()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32FileGroupDirectory::~Win32FileGroupDirectory()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::initialize( const ConstString & _name, const FileGroupInterfacePtr & _category, const FilePath & _folderPath )
    {
        m_name = _name;
        m_category = _category;
        m_folderPath = _folderPath;

        FilePath baseDirectoryPath;

        if( this->createDirectory( baseDirectoryPath ) == false )
        {
            LOGGER_ERROR( "Win32FileGroupDirectory::initialize: invalid create directory '%s'"
                , _folderPath.c_str()
            );

            return false;
        }

        m_factoryInputStream = new FactoryPool<Win32FileInputStream, 8>();
        m_factoryOutputStream = new FactoryPool<Win32FileOutputStream, 4>();
        m_factoryWin32MappedFile = new FactoryPool<Win32FileMappedStream, 4>();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32FileGroupDirectory::finalize()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryInputStream );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryOutputStream );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryWin32MappedFile );

        m_factoryInputStream = nullptr;
        m_factoryOutputStream = nullptr;
        m_factoryWin32MappedFile = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & Win32FileGroupDirectory::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    const FileGroupInterfacePtr & Win32FileGroupDirectory::getCategory() const
    {
        return m_category;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::isPacked() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & Win32FileGroupDirectory::getFolderPath() const
    {
        return m_folderPath;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::existFile( const FilePath & _fileName ) const
    {
        WChar filePath[MENGINE_MAX_PATH];
        if( Helper::Win32ConcatenateFilePath( m_relationPath, m_folderPath, _fileName, filePath, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR( "Win32FileSystem::existFile invlalid concatenate filePath '%s':'%s'"
                , m_folderPath.c_str()
                , _fileName.c_str()
            );

            return false;
        }

        if( PLATFORM_SERVICE()
            ->existFile( filePath ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::existDirectory( const FilePath & _folderName ) const
    {
        const FilePath & relationPath = this->getRelationPath();
        const FilePath & folderPath = this->getFolderPath();

        PathString accountString;
        accountString.append( relationPath );
        accountString.append( folderPath );
        accountString.append( _folderName );
        accountString.append( '/' );

        WString unicode_folderPath;
        if( Helper::utf8ToUnicode( accountString, unicode_folderPath ) == false )
        {
            return false;
        }

        if( PLATFORM_SERVICE()
            ->existDirectory( unicode_folderPath ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::createDirectory( const FilePath & _folderName ) const
    {
        const FilePath & relationPath = this->getRelationPath();
        const FilePath & folderPath = this->getFolderPath();

        PathString accountString;
        accountString.append( relationPath );
        accountString.append( folderPath );
        accountString.append( _folderName );

        WString unicode_folderPath;
        if( Helper::utf8ToUnicode( accountString, unicode_folderPath ) == false )
        {
            return false;
        }

        if( PLATFORM_SERVICE()
            ->existDirectory( unicode_folderPath ) == true )
        {
            return true;
        }

        if( PLATFORM_SERVICE()
            ->createDirectory( unicode_folderPath ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr Win32FileGroupDirectory::createInputFile( const FilePath & _fileName, bool _streaming )
    {
        (void)_fileName;
        (void)_streaming;

        Win32FileInputStreamPtr inputStream = m_factoryInputStream->createObject();

        return inputStream;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::openInputFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, bool _streaming )
    {
        (void)_streaming;

        if( _stream == nullptr )
        {
            LOGGER_ERROR( "Win32FileGroupDirectory::openInputFile failed _stream == NULL"
            );

            return false;
        }

        FileInputStreamInterface * file = stdex::intrusive_get<FileInputStreamInterface *>( _stream );

        if( file->open( m_relationPath, m_folderPath, _filePath, _offset, _size ) == false )
        {
            LOGGER_ERROR( "Win32FileGroupDirectory::openInputFile failed open file '%s':'%s'"
                , m_folderPath.c_str()
                , _filePath.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    OutputStreamInterfacePtr Win32FileGroupDirectory::createOutputFile()
    {
        Win32FileOutputStreamPtr outputStream = m_factoryOutputStream->createObject();

        return outputStream;
    }
    //////////////////////////////////////////////////////////////////////////	
    bool Win32FileGroupDirectory::openOutputFile( const FilePath & _filePath, const OutputStreamInterfacePtr & _stream )
    {
        if( _stream == nullptr )
        {
            LOGGER_ERROR( "Win32FileGroupDirectory::openOutputFile failed _stream == NULL"
            );

            return false;
        }

        FileOutputStreamInterface * file = stdex::intrusive_get<FileOutputStreamInterface *>( _stream );

        if( file->open( m_relationPath, m_folderPath, _filePath ) == false )
        {
            LOGGER_ERROR( "Win32FileGroupDirectory::openOutputFile failed open file '%s':'%s'"
                , m_folderPath.c_str()
                , _filePath.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32FileGroupDirectory::setRelationPath( const FilePath & _relationPath )
    {
        m_relationPath = _relationPath;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & Win32FileGroupDirectory::getRelationPath() const
    {
        return m_relationPath;
    }
    //////////////////////////////////////////////////////////////////////////
}
