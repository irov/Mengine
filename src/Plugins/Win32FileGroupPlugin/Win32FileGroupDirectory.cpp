#include "Win32FileGroupDirectory.h"

#include "Interface/MemoryInterface.h"
#include "Interface/PlatformInterface.h"

#include "Win32FileHelper.h"

#include "Kernel/UnicodeHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/PathHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/Stringalized.h"

#include "Environment/Windows/WindowsIncluder.h"

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
    bool Win32FileGroupDirectory::initialize( const ConstString & _name, const FileGroupInterfacePtr & _fileGroup, const FilePath & _folderPath )
    {
        m_name = _name;
        m_fileGroup = _fileGroup;
        m_folderPath = _folderPath;

        m_factoryInputStream = Helper::makeFactoryPool<Win32FileInputStream, 8>();
        m_factoryOutputStream = Helper::makeFactoryPool<Win32FileOutputStream, 4>();
        m_factoryWin32MappedFile = Helper::makeFactoryPool<Win32FileMappedStream, 4>();

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
    const FileGroupInterfacePtr & Win32FileGroupDirectory::getFileGroup() const
    {
        return m_fileGroup;
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
    bool Win32FileGroupDirectory::existFile( const FilePath & _filePath ) const
    {
        WChar unicode_filePath[MENGINE_MAX_PATH];
        size_t unicode_filePathLen = Helper::Win32ConcatenateFilePathW( m_relationPath, m_folderPath, _filePath, unicode_filePath, MENGINE_MAX_PATH );

        if( unicode_filePathLen == MENGINE_PATH_INVALID_LENGTH )
        {
            LOGGER_ERROR( "invlalid concatenate filePath '%s':'%s'"
                , m_folderPath.c_str()
                , _filePath.c_str()
            );

            return false;
        }

        Char utf8_filePath[MENGINE_MAX_PATH];
        if( Helper::unicodeToUtf8Size( unicode_filePath, unicode_filePathLen, utf8_filePath, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        bool result = PLATFORM_SERVICE()
            ->existFile( utf8_filePath );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::existDirectory( const FilePath & _folderName ) const
    {
        const FilePath & relationPath = this->getRelationPath();
        const FilePath & folderPath = this->getFolderPath();

        PathString account_folderPath;
        account_folderPath.append( relationPath );
        account_folderPath.append( folderPath );
        account_folderPath.append( _folderName );
        account_folderPath.append( '/' );

        if( PLATFORM_SERVICE()
            ->existDirectory( account_folderPath.c_str() ) == false )
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

        PathString account_folderPath;
        account_folderPath.append( relationPath );
        account_folderPath.append( folderPath );
        account_folderPath.append( _folderName );

        if( PLATFORM_SERVICE()
            ->existDirectory( account_folderPath.c_str() ) == true )
        {
            return true;
        }

        if( PLATFORM_SERVICE()
            ->createDirectory( account_folderPath.c_str() ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::findFiles( const FilePath & _filePath, const Char * _mask, const LambdaFilePath & _lambda ) const
    {
        Char unicode_base[MENGINE_MAX_PATH];
        size_t fullPath = Helper::Win32ConcatenateFilePathA( m_relationPath, m_folderPath, FilePath::none(), unicode_base, MENGINE_MAX_PATH );
        
        MENGINE_UNUSED( fullPath );

        MENGINE_ASSERTION_FATAL_RETURN( fullPath != MENGINE_PATH_INVALID_LENGTH, false, "invlalid concatenate filePath '%s':'%s'"
            , m_folderPath.c_str()
            , _filePath.c_str()
        );

        Helper::pathCorrectForwardslashA( unicode_base );

        PLATFORM_SERVICE()
            ->findFiles( unicode_base, _filePath.c_str(), _mask, _lambda );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr Win32FileGroupDirectory::createInputFile( const FilePath & _filePath, bool _streaming, const Char * _doc )
    {
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _streaming );

        Win32FileInputStreamPtr inputStream = m_factoryInputStream->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( inputStream, nullptr );

        return inputStream;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::openInputFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, bool _streaming )
    {
        MENGINE_UNUSED( _streaming );

        MENGINE_ASSERTION_MEMORY_PANIC( _stream, false, "failed _stream == NULL" );

        FileInputStreamInterface * file = stdex::intrusive_get<FileInputStreamInterface *>( _stream );

        if( file->open( m_relationPath, m_folderPath, _filePath, _offset, _size, _streaming ) == false )
        {
            LOGGER_ERROR( "failed open file '%s':'%s'"
                , m_folderPath.c_str()
                , _filePath.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    OutputStreamInterfacePtr Win32FileGroupDirectory::createOutputFile( const Char * _doc )
    {
        Win32FileOutputStreamPtr outputStream = m_factoryOutputStream->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( outputStream, nullptr );

        return outputStream;
    }
    //////////////////////////////////////////////////////////////////////////	
    bool Win32FileGroupDirectory::openOutputFile( const FilePath & _filePath, const OutputStreamInterfacePtr & _stream )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, false, "failed _stream == NULL" );

        FileOutputStreamInterface * file = stdex::intrusive_get<FileOutputStreamInterface *>( _stream );

        if( file->open( m_relationPath, m_folderPath, _filePath ) == false )
        {
            LOGGER_ERROR( "failed open file '%s':'%s'"
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
