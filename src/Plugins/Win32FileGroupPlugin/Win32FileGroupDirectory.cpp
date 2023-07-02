#include "Win32FileGroupDirectory.h"

#include "Interface/MemoryInterface.h"
#include "Interface/PlatformServiceInterface.h"

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
#include "Kernel/DocumentHelper.h"
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
        m_factoryInputStreamFile = Helper::makeFactoryPool<Win32FileInputStream, 8>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryInputStreamMutexFile = Helper::makeFactoryPool<Win32MutexFileInputStream, 8>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryOutputStreamFile = Helper::makeFactoryPool<Win32FileOutputStream, 4>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryMappedFile = Helper::makeFactoryPool<Win32FileMapped, 4>( MENGINE_DOCUMENT_FACTORABLE );

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
        size_t fullPathLen = Helper::Win32ConcatenateFilePathA( m_relationPath, m_folderPath, _filePath, _fullPath, MENGINE_MAX_PATH );

        MENGINE_UNUSED( fullPathLen );

        MENGINE_ASSERTION_FATAL( fullPathLen != MENGINE_PATH_INVALID_LENGTH, "invlalid concatenate fullPath '%s':'%s'"
            , m_folderPath.c_str()
            , _filePath.c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::existFile( const FilePath & _filePath, bool _recursive ) const
    {
        Char utf8_filePath[MENGINE_MAX_PATH] = {'\0'};
        this->getFullPath( _filePath, utf8_filePath );

        bool result = PLATFORM_SERVICE()
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

        bool successful = PLATFORM_SERVICE()
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

        bool result = PLATFORM_SERVICE()
            ->existDirectory( basePath.c_str(), _folderPath.c_str() );

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

        PathString basePath;
        basePath.append( relationPath );
        basePath.append( folderPath );

        if( PLATFORM_SERVICE()
            ->existDirectory( basePath.c_str(), _folderName.c_str() ) == true )
        {
            return true;
        }

        if( PLATFORM_SERVICE()
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

        bool successful = PLATFORM_SERVICE()
            ->moveFile( oldFilePathString.c_str(), newFilePathString.c_str() );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::findFiles( const FilePath & _filePath, const Char * _mask, const LambdaFilePath & _lambda ) const
    {
        Char fullPathBase[MENGINE_MAX_PATH] = {'\0'};
        this->getFullPath( FilePath::none(), fullPathBase );

        Helper::pathCorrectForwardslashA( fullPathBase );

        PLATFORM_SERVICE()
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

        MENGINE_ASSERTION_MEMORY_PANIC( stream );

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

        MENGINE_ASSERTION( result == true, "failed open file '%s':'%s'"
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

        MENGINE_ASSERTION_MEMORY_PANIC( stream );

        stream->initialize( _stream, _mutex );

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

        MENGINE_ASSERTION( result == true, "failed open file '%s':'%s'"
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

        MENGINE_ASSERTION( result == true, "failed close file '%s'"
            , m_folderPath.c_str()
        );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    OutputStreamInterfacePtr Win32FileGroupDirectory::createOutputFile( const DocumentInterfacePtr & _doc )
    {
        Win32FileOutputStreamPtr stream = m_factoryOutputStreamFile->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( stream );

        return stream;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::openOutputFile( const FilePath & _filePath, const OutputStreamInterfacePtr & _stream, bool _withTemp )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, "failed _stream == nullptr" );

        FileOutputStreamInterface * file = _stream.getT<FileOutputStreamInterface *>();

        bool result = file->open( m_relationPath, m_folderPath, _filePath, _withTemp );

        MENGINE_ASSERTION( result == true, "failed open file '%s':'%s'"
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

        MENGINE_ASSERTION( result == true, "failed close file '%s'"
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

        MENGINE_ASSERTION_MEMORY_PANIC( mapped );

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

        MENGINE_ASSERTION( result == true, "failed mapped file '%s':'%s'"
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

        MENGINE_ASSERTION( result == true, "failed close file '%s'"
            , m_folderPath.c_str()
        );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
}
