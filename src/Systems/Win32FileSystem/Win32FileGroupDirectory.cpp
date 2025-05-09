#include "Win32FileGroupDirectory.h"

#include "Interface/MemoryInterface.h"
#include "Interface/FileSystemInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Win32ConcatenateFileHelper.h"
#include "Win32FileInputStream.h"
#include "Win32FileOutputStream.h"
#include "Win32MutexFileInputStream.h"
#include "Win32FileMapped.h"

#include "Kernel/PathString.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/PathHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Stringalized.h"

#include "Config/Path.h"

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
        m_factoryInputStreamFile = Helper::makeFactoryPoolWithMutex<Win32FileInputStream, 8>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryInputStreamMutexFile = Helper::makeFactoryPoolWithMutex<Win32MutexFileInputStream, 8>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryOutputStreamFile = Helper::makeFactoryPoolWithMutex<Win32FileOutputStream, 4>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryMappedFile = Helper::makeFactoryPoolWithMutex<Win32FileMapped, 4>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32FileGroupDirectory::_finalize()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryInputStreamFile );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryInputStreamMutexFile );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryOutputStreamFile );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryMappedFile );

        m_factoryInputStreamFile = nullptr;
        m_factoryInputStreamMutexFile = nullptr;
        m_factoryOutputStreamFile = nullptr;
        m_factoryMappedFile = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::isPacked() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32FileGroupDirectory::getFullPath( const FilePath & _filePath, Char * const _fullPath ) const
    {
        size_t fullPathLen = Helper::Win32ConcatenateFilePathA( m_relationPath, m_folderPath, _filePath, _fullPath );

        MENGINE_UNUSED( fullPathLen );

        MENGINE_ASSERTION_FATAL( fullPathLen != MENGINE_PATH_INVALID_LENGTH, "invlalid concatenate fullPath '%s%s%s'"
            , m_relationPath.c_str()
            , m_folderPath.c_str()
            , _filePath.c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::existFile( const FilePath & _filePath, bool _recursive ) const
    {
        Path utf8_filePath = {'\0'};
        this->getFullPath( _filePath, utf8_filePath );

        bool result = FILE_SYSTEM()
            ->existFile( utf8_filePath );

        if( _recursive == true && result == false && m_parentFileGroup != nullptr )
        {
            result = m_parentFileGroup->existFile( _filePath, true );
        }

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::removeFile( const FilePath & _filePath ) const
    {
        const FilePath & relationPath = this->getRelationPath();
        const FilePath & folderPath = this->getFolderPath();

        PathString filePathString;
        filePathString.append( relationPath );
        filePathString.append( folderPath );
        filePathString.append( _filePath );

        bool successful = FILE_SYSTEM()
            ->removeFile( filePathString.c_str() );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::existDirectory( const FilePath & _folderPath, bool _recursive ) const
    {
        const FilePath & relationPath = this->getRelationPath();
        const FilePath & folderPath = this->getFolderPath();

        PathString basePath;
        basePath.append( relationPath );
        basePath.append( folderPath );

        if( FILE_SYSTEM()
            ->existDirectory( basePath.c_str(), _folderPath.c_str() ) == true )
        {
            return true;
        }

        if( _recursive == true && m_parentFileGroup != nullptr )
        {
            if( m_parentFileGroup->existDirectory( _folderPath, true ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::createDirectory( const FilePath & _folderName ) const
    {
        const FilePath & relationPath = this->getRelationPath();
        const FilePath & folderPath = this->getFolderPath();

        PathString basePath;
        basePath.append( relationPath );
        basePath.append( folderPath );

        if( FILE_SYSTEM()
            ->existDirectory( basePath.c_str(), _folderName.c_str() ) == true )
        {
            return true;
        }

        if( FILE_SYSTEM()
            ->createDirectory( basePath.c_str(), _folderName.c_str() ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::moveFile( const FilePath & _oldFilePath, const FilePath & _newFilePath ) const
    {
        const FilePath & relationPath = this->getRelationPath();
        const FilePath & folderPath = this->getFolderPath();

        PathString oldFilePathString;
        oldFilePathString.append( relationPath );
        oldFilePathString.append( folderPath );
        oldFilePathString.append( _oldFilePath );

        PathString newFilePathString;
        newFilePathString.append( relationPath );
        newFilePathString.append( folderPath );
        newFilePathString.append( _newFilePath );

        bool successful = FILE_SYSTEM()
            ->moveFile( oldFilePathString.c_str(), newFilePathString.c_str() );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::findFiles( const FilePath & _filePath, const Char * _mask, const LambdaFilePath & _lambda ) const
    {
        Path fullPathBase = {'\0'};
        this->getFullPath( FilePath::none(), fullPathBase );

        Helper::pathCorrectForwardslashA( fullPathBase );

        FILE_SYSTEM()
            ->findFiles( fullPathBase, _filePath.c_str(), _mask, _lambda );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr Win32FileGroupDirectory::createInputFile( const FilePath & _filePath, bool _streaming, FileGroupInterface ** const _fileGroup, const DocumentInterfacePtr & _doc )
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

        Win32FileInputStreamPtr stream = m_factoryInputStreamFile->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "invalid create input file" );

        if( _fileGroup != nullptr )
        {
            *_fileGroup = this;
        }

        return stream;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::openInputFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, bool _streaming, bool _share )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, "failed _stream == nullptr" );

        FileInputStreamInterface * file = _stream.getT<FileInputStreamInterface *>();

        bool result = file->open( m_relationPath, m_folderPath, _filePath, _offset, _size, _streaming, _share );

        MENGINE_ASSERTION( result == true, "failed open file '%s%s%s'"
            , m_relationPath.c_str()
            , m_folderPath.c_str()
            , _filePath.c_str()
        );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::closeInputFile( const InputStreamInterfacePtr & _stream )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, "failed _stream == nullptr" );

        FileInputStreamInterface * file = _stream.getT<FileInputStreamInterface *>();

        bool result = file->close();

        MENGINE_ASSERTION( result == true, "failed close file '%s'"
            , m_folderPath.c_str()
        );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr Win32FileGroupDirectory::createInputMutexFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, const ThreadMutexInterfacePtr & _mutex, FileGroupInterface ** const _fileGroup, const DocumentInterfacePtr & _doc )
    {
        if( m_parentFileGroup != nullptr )
        {
            if( this->existFile( _filePath, false ) == false )
            {
                InputStreamInterfacePtr stream = m_parentFileGroup->createInputMutexFile( _filePath, _stream, _mutex, _fileGroup, _doc );

                return stream;
            }
        }

        Win32MutexFileInputStreamPtr stream = m_factoryInputStreamMutexFile->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "invalid create input mutex file" );

        stream->setFileInputStream( _stream );
        stream->setThreadMutex( _mutex );

        if( _fileGroup != nullptr )
        {
            *_fileGroup = this;
        }

        return stream;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::openInputMutexFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, "failed _stream == nullptr" );

        FileInputStreamInterface * file = _stream.getT<FileInputStreamInterface *>();

        bool result = file->open( m_relationPath, m_folderPath, _filePath, _offset, _size, false, false );

        MENGINE_ASSERTION( result == true, "failed open file '%s%s%s'"
            , m_relationPath.c_str()
            , m_folderPath.c_str()
            , _filePath.c_str()
        );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::closeInputMutexFile( const InputStreamInterfacePtr & _stream )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, "failed _stream == nullptr" );

        FileInputStreamInterface * file = _stream.getT<FileInputStreamInterface *>();

        bool result = file->close();

        MENGINE_ASSERTION( result == true, "failed close file '%s%s'"
            , m_relationPath.c_str()
            , m_folderPath.c_str()
        );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    OutputStreamInterfacePtr Win32FileGroupDirectory::createOutputFile( const DocumentInterfacePtr & _doc )
    {
        Win32FileOutputStreamPtr stream = m_factoryOutputStreamFile->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "invalid create output file" );

        return stream;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::openOutputFile( const FilePath & _filePath, const OutputStreamInterfacePtr & _stream, bool _withTemp )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, "failed _stream == nullptr" );

        FileOutputStreamInterface * file = _stream.getT<FileOutputStreamInterface *>();

        bool result = file->open( m_relationPath, m_folderPath, _filePath, _withTemp );

        MENGINE_ASSERTION( result == true, "failed open file '%s%s%s'"
            , m_relationPath.c_str()
            , m_folderPath.c_str()
            , _filePath.c_str()
        );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::closeOutputFile( const OutputStreamInterfacePtr & _stream )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, "failed _stream == nullptr" );

        FileOutputStreamInterface * file = _stream.getT<FileOutputStreamInterface *>();

        bool result = file->close();

        MENGINE_ASSERTION( result == true, "failed close file '%s%s'"
            , m_relationPath.c_str()
            , m_folderPath.c_str()
        );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::isAvailableMappedFile() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    MappedInterfacePtr Win32FileGroupDirectory::createMappedFile( const FilePath & _filePath, FileGroupInterface ** const _fileGroup, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _filePath );

        if( m_parentFileGroup != nullptr )
        {
            if( this->existFile( _filePath, false ) == false )
            {
                MappedInterfacePtr mapped = m_parentFileGroup->createMappedFile( _filePath, _fileGroup, _doc );

                return mapped;
            }
        }

        Win32FileMappedPtr mapped = m_factoryMappedFile->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( mapped, "invalid create mapped file" );

        if( _fileGroup != nullptr )
        {
            *_fileGroup = this;
        }

        return mapped;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::openMappedFile( const FilePath & _filePath, const MappedInterfacePtr & _stream, bool _shared )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, "failed _stream == nullptr" );

        FileMappedInterface * mapped = _stream.getT<FileMappedInterface *>();

        bool result = mapped->open( m_relationPath, m_folderPath, _filePath, _shared );

        MENGINE_ASSERTION( result == true, "failed mapped file '%s%s%s'"
            , m_relationPath.c_str()
            , m_folderPath.c_str()
            , _filePath.c_str()
        );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::closeMappedFile( const MappedInterfacePtr & _stream )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, "failed _stream == nullptr" );

        FileMappedInterface * mapped = _stream.getT<FileMappedInterface *>();

        bool result = mapped->close();

        MENGINE_ASSERTION( result == true, "failed close file '%s%s'"
            , m_relationPath.c_str()
            , m_folderPath.c_str()
        );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
}
