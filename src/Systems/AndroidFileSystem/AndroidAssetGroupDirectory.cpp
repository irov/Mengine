#include "AndroidAssetGroupDirectory.h"

#include "Interface/PlatformServiceInterface.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidActivityHelper.h"
#include "Environment/Android/AndroidAssetServiceInterface.h"

#include "AndroidAssetInputStream.h"
#include "AndroidMutexAssetInputStream.h"

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
    AndroidAssetGroupDirectory::AndroidAssetGroupDirectory()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidAssetGroupDirectory::~AndroidAssetGroupDirectory()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidAssetGroupDirectory::_initialize()
    {
        m_factoryInputStreamFile = Helper::makeFactoryPoolWithMutex<AndroidAssetInputStream, 8>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryInputStreamMutexFile = Helper::makeFactoryPoolWithMutex<AndroidMutexAssetInputStream, 8>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidAssetGroupDirectory::_finalize()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryInputStreamFile );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryInputStreamMutexFile );

        m_factoryInputStreamFile = nullptr;
        m_factoryInputStreamMutexFile = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidAssetGroupDirectory::isPacked() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidAssetGroupDirectory::getFullPath( const FilePath & _filePath, Char * const _fullPath ) const
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
    bool AndroidAssetGroupDirectory::existFile( const FilePath & _filePath, bool _recursive ) const
    {
        Path fullPath = {'\0'};
        this->getFullPath( _filePath, fullPath );

        if( ANDROID_ASSET_SERVICE()
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
    bool AndroidAssetGroupDirectory::removeFile( const FilePath & _filePath ) const
    {
        MENGINE_UNUSED( _filePath );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidAssetGroupDirectory::existDirectory( const FilePath & _folderName, bool _recursive ) const
    {
        MENGINE_UNUSED( _folderName );
        MENGINE_UNUSED( _recursive );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidAssetGroupDirectory::createDirectory( const FilePath & _folderName ) const
    {
        MENGINE_UNUSED( _folderName );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidAssetGroupDirectory::moveFile( const FilePath & _oldFilePath, const FilePath & _newFilePath ) const
    {
        MENGINE_UNUSED( _oldFilePath );
        MENGINE_UNUSED( _newFilePath );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidAssetGroupDirectory::findFiles( const FilePath & _filePath, const Char * _mask, const LambdaFilePath & _lambda ) const
    {
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _mask );
        MENGINE_UNUSED( _lambda );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr AndroidAssetGroupDirectory::createInputFile( const FilePath & _filePath, bool _streaming, FileGroupInterface ** const _fileGroup, const DocumentInterfacePtr & _doc )
    {
        if( m_parentFileGroup != nullptr )
        {
            if( this->existFile( _filePath, false ) == false )
            {
                InputStreamInterfacePtr stream = m_parentFileGroup->createInputFile( _filePath, _streaming, _fileGroup, _doc );

                return stream;
            }
        }

        AndroidAssetInputStreamPtr stream = m_factoryInputStreamFile->createObject( _doc );
        
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
    bool AndroidAssetGroupDirectory::openInputFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, bool _streaming, bool _share )
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
    void AndroidAssetGroupDirectory::closeInputFile( const InputStreamInterfacePtr & _stream )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, "failed _stream == nullptr" );

        FileInputStreamInterface * file = stdex::intrusive_get<FileInputStreamInterface *>( _stream );

        file->close();
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr AndroidAssetGroupDirectory::createInputMutexFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, const ThreadMutexInterfacePtr & _mutex, FileGroupInterface ** const _fileGroup, const DocumentInterfacePtr & _doc )
    {
        if( m_parentFileGroup != nullptr )
        {
            if( this->existFile( _filePath, false ) == false )
            {
                InputStreamInterfacePtr stream = m_parentFileGroup->createInputMutexFile( _filePath, _stream, _mutex, _fileGroup, _doc );

                return stream;
            }
        }

        AndroidMutexAssetInputStreamPtr stream = m_factoryInputStreamMutexFile->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "invalid create input mutex file '%s:%s'"
            , m_folderPath.c_str()
            , _filePath.c_str()
        );

        stream->setAssetInputStream( _stream );
        stream->setThreadMutex( _mutex );

        if( _fileGroup != nullptr )
        {
            *_fileGroup = this;
        }

        return stream;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidAssetGroupDirectory::openInputMutexFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size )
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
    void AndroidAssetGroupDirectory::closeInputMutexFile( const InputStreamInterfacePtr & _stream )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, "failed _stream == nullptr" );

        FileInputStreamInterface * file = stdex::intrusive_get<FileInputStreamInterface *>( _stream );

        file->close();
    }
    //////////////////////////////////////////////////////////////////////////
    OutputStreamInterfacePtr AndroidAssetGroupDirectory::createOutputFile( const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////	
    bool AndroidAssetGroupDirectory::openOutputFile( const FilePath & _filePath, const OutputStreamInterfacePtr & _stream, bool _withTemp )
    {
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _stream );
        MENGINE_UNUSED( _withTemp );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidAssetGroupDirectory::closeOutputFile( const OutputStreamInterfacePtr & _stream )
    {
        MENGINE_UNUSED( _stream );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidAssetGroupDirectory::isAvailableMappedFile() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    MappedInterfacePtr AndroidAssetGroupDirectory::createMappedFile( const FilePath & _filePath, FileGroupInterface ** const _fileGroup, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _fileGroup );
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidAssetGroupDirectory::openMappedFile( const FilePath & _filePath, const MappedInterfacePtr & _stream, bool _shared )
    {
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _stream );
        MENGINE_UNUSED( _shared );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidAssetGroupDirectory::closeMappedFile( const MappedInterfacePtr & _stream )
    {
        MENGINE_UNUSED( _stream );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();
    }
    //////////////////////////////////////////////////////////////////////////
}
