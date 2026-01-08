#include "SDL3FileGroupDirectory.h"

#include "Interface/FileSystemInterface.h"

#include "Environment/SDL3/SDL3Includer.h"

#include "SDL3FileInputStream.h"
#include "SDL3MutexFileInputStream.h"
#include "SDL3FileOutputStream.h"
#include "SDL3FileMapped.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/PathHelper.h"
#include "Kernel/PathString.h"
#include "Kernel/DocumentHelper.h"

#include "Config/Path.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDL3FileGroupDirectory::SDL3FileGroupDirectory()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDL3FileGroupDirectory::~SDL3FileGroupDirectory()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3FileGroupDirectory::_initialize()
    {
        m_factoryInputStreamFile = Helper::makeFactoryPoolWithMutex<SDL3FileInputStream, 8>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryInputStreamMutexFile = Helper::makeFactoryPoolWithMutex<SDL3MutexFileInputStream, 8>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryOutputStreamFile = Helper::makeFactoryPoolWithMutex<SDL3FileOutputStream, 4>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3FileGroupDirectory::_finalize()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryInputStreamFile );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryInputStreamMutexFile );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryOutputStreamFile );

        m_factoryInputStreamFile = nullptr;
        m_factoryInputStreamMutexFile = nullptr;
        m_factoryOutputStreamFile = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3FileGroupDirectory::isPacked() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3FileGroupDirectory::getFullPath( const FilePath & _filePath, Char * const _fullPath ) const
    {
        bool successful = Helper::concatenateFilePath( {m_relationPath, m_folderPath, _filePath}, _fullPath );

        MENGINE_UNUSED( successful );

        MENGINE_ASSERTION_FATAL( successful == true, "invalid concatenate fullPath relation '%s' folder '%s' file '%s'"
            , m_relationPath.c_str()
            , m_folderPath.c_str()
            , _filePath.c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3FileGroupDirectory::existFile( const FilePath & _filePath, bool _recursive ) const
    {
        Path fullPath = {'\0'};
        this->getFullPath( _filePath, fullPath );

        if( FILE_SYSTEM()
            ->existFile( fullPath ) == true )
        {
            return true;
        }

        if( _recursive == true && m_parentFileGroup != nullptr )
        {
            bool result = m_parentFileGroup->existFile( _filePath, true );

            return result;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3FileGroupDirectory::removeFile( const FilePath & _filePath ) const
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
    bool SDL3FileGroupDirectory::existDirectory( const FilePath & _folderName, bool _recursive ) const
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

        if( _recursive == true && m_parentFileGroup != nullptr )
        {
            if( m_parentFileGroup->existDirectory( _folderName, true ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3FileGroupDirectory::createDirectory( const FilePath & _folderName ) const
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
    bool SDL3FileGroupDirectory::moveFile( const FilePath & _oldFilePath, const FilePath & _newFilePath ) const
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
    bool SDL3FileGroupDirectory::findFiles( const FilePath & _filePath, const Char * _mask, const LambdaFilePath & _lambda ) const
    {
        Path utf8_base = {'\0'};
        if( Helper::concatenateFilePath( {m_relationPath, m_folderPath, FilePath::none()}, utf8_base ) == false )
        {
            LOGGER_ERROR( "invalid concatenate filePath '%s:%s'"
                , m_folderPath.c_str()
                , _filePath.c_str()
            );

            return false;
        }

        Helper::pathCorrectForwardslashA( utf8_base );

        if( FILE_SYSTEM()
            ->findFiles( utf8_base, _filePath.c_str(), _mask, _lambda ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr SDL3FileGroupDirectory::createInputFile( const FilePath & _filePath, bool _streaming, FileGroupInterface ** const _fileGroup, const DocumentInterfacePtr & _doc )
    {
        if( m_parentFileGroup != nullptr )
        {
            if( this->existFile( _filePath, false ) == false )
            {
                InputStreamInterfacePtr stream = m_parentFileGroup->createInputFile( _filePath, _streaming, _fileGroup, _doc );

                return stream;
            }
        }

        SDL3FileInputStreamPtr stream = m_factoryInputStreamFile->createObject( _doc );
        
        MENGINE_ASSERTION_MEMORY_PANIC( stream, "invalid create input file '%s:%s'"
            , m_folderPath.c_str()
            , _filePath.c_str()
        );

        if( _fileGroup != nullptr )
        {
            *_fileGroup = this;
        }

        return stream;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3FileGroupDirectory::openInputFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, bool _streaming, bool _share )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, "failed _stream == nullptr" );

        FileInputStreamInterface * file = stdex::intrusive_get<FileInputStreamInterface *>( _stream );

        bool result = file->open( m_relationPath, m_folderPath, _filePath, _offset, _size, _streaming, _share );
        
        MENGINE_ASSERTION( result == true, "failed open input file '%s:%s'"
            , m_folderPath.c_str()
            , _filePath.c_str()
        );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3FileGroupDirectory::closeInputFile( const InputStreamInterfacePtr & _stream )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, "failed _stream == nullptr" );

        FileInputStreamInterface * file = stdex::intrusive_get<FileInputStreamInterface *>( _stream );

        file->close();
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr SDL3FileGroupDirectory::createInputMutexFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, const ThreadMutexInterfacePtr & _mutex, FileGroupInterface ** const _fileGroup, const DocumentInterfacePtr & _doc )
    {
        if( m_parentFileGroup != nullptr )
        {
            if( this->existFile( _filePath, false ) == false )
            {
                InputStreamInterfacePtr stream = m_parentFileGroup->createInputMutexFile( _filePath, _stream, _mutex, _fileGroup, _doc );

                return stream;
            }
        }

        SDL3MutexFileInputStreamPtr stream = m_factoryInputStreamMutexFile->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "invalid create input mutex file '%s:%s'"
            , m_folderPath.c_str()
            , _filePath.c_str()
        );

        stream->initialize( _stream, _mutex );

        if( _fileGroup != nullptr )
        {
            *_fileGroup = this;
        }

        return stream;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3FileGroupDirectory::openInputMutexFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, "failed _stream == nullptr" );

        FileInputStreamInterface * file = stdex::intrusive_get<FileInputStreamInterface *>( _stream );

        bool result = file->open( m_relationPath, m_folderPath, _filePath, _offset, _size, false, false );

        MENGINE_ASSERTION( result == true, "failed open input mutex file '%s:%s'"
            , m_folderPath.c_str()
            , _filePath.c_str()
        );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3FileGroupDirectory::closeInputMutexFile( const InputStreamInterfacePtr & _stream )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, "failed _stream == nullptr" );

        FileInputStreamInterface * file = stdex::intrusive_get<FileInputStreamInterface *>( _stream );

        file->close();
    }
    //////////////////////////////////////////////////////////////////////////
    OutputStreamInterfacePtr SDL3FileGroupDirectory::createOutputFile( const DocumentInterfacePtr & _doc )
    {
        SDL3FileOutputStreamPtr stream = m_factoryOutputStreamFile->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "invalid create output file" );

        return stream;
    }
    //////////////////////////////////////////////////////////////////////////	
    bool SDL3FileGroupDirectory::openOutputFile( const FilePath & _filePath, const OutputStreamInterfacePtr & _stream, bool _withTemp )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, "failed _stream == nullptr" );

        FileOutputStreamInterface * file = stdex::intrusive_get<FileOutputStreamInterface *>( _stream );

        bool result = file->open( m_relationPath, m_folderPath, _filePath, _withTemp );
        
        MENGINE_ASSERTION( result == true, "failed open output file '%s:%s'"
            , m_folderPath.c_str()
            , _filePath.c_str()
        );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3FileGroupDirectory::closeOutputFile( const OutputStreamInterfacePtr & _stream )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, "failed _stream == nullptr" );

        FileOutputStreamInterface * file = stdex::intrusive_get<FileOutputStreamInterface *>( _stream );

        bool result = file->close();

        MENGINE_ASSERTION( result == true, "failed close file '%s'"
            , m_folderPath.c_str()
        );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3FileGroupDirectory::isAvailableMappedFile() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    MappedInterfacePtr SDL3FileGroupDirectory::createMappedFile( const FilePath & _filePath, FileGroupInterface ** const _fileGroup, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _fileGroup );
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3FileGroupDirectory::openMappedFile( const FilePath & _filePath, const MappedInterfacePtr & _stream, bool _shared )
    {
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _stream );
        MENGINE_UNUSED( _shared );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3FileGroupDirectory::closeMappedFile( const MappedInterfacePtr & _stream )
    {
        MENGINE_UNUSED( _stream );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();
    }
    //////////////////////////////////////////////////////////////////////////
}