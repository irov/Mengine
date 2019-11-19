#include "Win32FileGroupDirectory.h"

#include "Interface/MemoryInterface.h"
#include "Interface/PlatformInterface.h"

#include "Win32FileHelper.h"

#include "Kernel/PathString.h"
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
    bool Win32FileGroupDirectory::_initialize()
    {
        m_factoryInputStream = Helper::makeFactoryPool<Win32FileInputStream, 8>();
        m_factoryOutputStream = Helper::makeFactoryPool<Win32FileOutputStream, 4>();
        m_factoryWin32MappedFile = Helper::makeFactoryPool<Win32FileMappedStream, 4>();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32FileGroupDirectory::_finalize()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryInputStream );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryOutputStream );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryWin32MappedFile );

        m_factoryInputStream = nullptr;
        m_factoryOutputStream = nullptr;
        m_factoryWin32MappedFile = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::isPacked() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::getFullPath( const FilePath & _filePath, Char * _fullPath ) const
    {
        size_t fullPathLen = Helper::Win32ConcatenateFilePathA( m_relationPath, m_folderPath, _filePath, _fullPath, MENGINE_MAX_PATH );

        MENGINE_UNUSED( fullPathLen );

        MENGINE_ASSERTION_FATAL_RETURN( fullPathLen != MENGINE_PATH_INVALID_LENGTH, false, "invlalid concatenate fullPath '%s':'%s'"
            , m_folderPath.c_str()
            , _filePath.c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::existFile( const FilePath & _filePath, bool _recursive ) const
    {
        Char utf8_filePath[MENGINE_MAX_PATH];
        if( this->getFullPath( _filePath, utf8_filePath ) == false )
        {
            LOGGER_ERROR( "invlalid get fullPath '%s'"
                , _filePath.c_str()
            );

            return false;
        }

        bool result = PLATFORM_SERVICE()
            ->existFile( utf8_filePath );

        if( _recursive == true && result == false && m_parentFileGroup != nullptr )
        {
            result = m_parentFileGroup->existFile( _filePath, true );
        }

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::existDirectory( const FilePath & _folderPath, bool _recursive ) const
    {
        const FilePath & relationPath = this->getRelationPath();
        const FilePath & folderPath = this->getFolderPath();

        PathString account_folderPath;
        account_folderPath.append( relationPath );
        account_folderPath.append( folderPath );
        account_folderPath.append( _folderPath );
        account_folderPath.append( '/' );

        bool result = PLATFORM_SERVICE()
            ->existDirectory( account_folderPath.c_str() );

        if( _recursive == true && result == false && m_parentFileGroup != nullptr )
        {
            result = m_parentFileGroup->existDirectory( _folderPath, true );
        }

        return result;
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
        Char fullPathBase[MENGINE_MAX_PATH];
        if( this->getFullPath( FilePath::none(), fullPathBase ) == false )
        {
            return false;
        }

        Helper::pathCorrectForwardslashA( fullPathBase );

        PLATFORM_SERVICE()
            ->findFiles( fullPathBase, _filePath.c_str(), _mask, _lambda );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr Win32FileGroupDirectory::createInputFile( const FilePath & _filePath, bool _streaming, FileGroupInterface ** _fileGroup, const Char * _doc )
    {
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _streaming );

        if( m_parentFileGroup != nullptr )
        {
            if( this->existFile( _filePath, false ) == false )
            {
                InputStreamInterfacePtr stream = m_parentFileGroup->createInputFile( _filePath, _streaming, _fileGroup, _doc );

                return stream;
            }
        }

        Win32FileInputStreamPtr stream = m_factoryInputStream->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, nullptr );

        if( _fileGroup != nullptr )
        {
            *_fileGroup = this;
        }

        return stream;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::openInputFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, bool _streaming )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, false, "failed _stream == nullptr" );

        FileInputStreamInterface * file = stdex::intrusive_get<FileInputStreamInterface *>( _stream );

        bool result = file->open( m_relationPath, m_folderPath, _filePath, _offset, _size, _streaming );

        MENGINE_ASSERTION_RETURN( result == true, false, "failed open file '%s':'%s'"
            , m_folderPath.c_str()
            , _filePath.c_str()
        );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    OutputStreamInterfacePtr Win32FileGroupDirectory::createOutputFile( const Char * _doc )
    {
        Win32FileOutputStreamPtr stream = m_factoryOutputStream->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, nullptr );

        return stream;
    }
    //////////////////////////////////////////////////////////////////////////	
    bool Win32FileGroupDirectory::openOutputFile( const FilePath & _filePath, const OutputStreamInterfacePtr & _stream )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, false, "failed _stream == nullptr" );

        FileOutputStreamInterface * file = stdex::intrusive_get<FileOutputStreamInterface *>( _stream );

        bool result = file->open( m_relationPath, m_folderPath, _filePath );
        
        MENGINE_ASSERTION_RETURN( result == true, false, "failed open file '%s':'%s'"
            , m_folderPath.c_str()
            , _filePath.c_str()
        );

        return result;
    }
}
