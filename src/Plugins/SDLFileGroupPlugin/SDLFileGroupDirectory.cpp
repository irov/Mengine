#include "SDLFileGroupDirectory.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/PlatformInterface.h"

#include "SDLFileInputStream.h"
#include "SDLFileOutputStream.h"
#include "SDLFileMappedStream.h"
#include "SDLFileHelper.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/PathHelper.h"
#include "Kernel/PathString.h"

#include "SDL_rwops.h"

namespace Mengine
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
    bool SDLFileGroupDirectory::_initialize()
    {
        m_factoryInputStream = Helper::makeFactoryPool<SDLFileInputStream, 8>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryOutputStream = Helper::makeFactoryPool<SDLFileOutputStream, 4>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLFileGroupDirectory::_finalize()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryInputStream );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryOutputStream );

        m_factoryInputStream = nullptr;
        m_factoryOutputStream = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileGroupDirectory::isPacked() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileGroupDirectory::getFullPath( const FilePath & _filePath, Char * _fullPath ) const
    {
        bool successful = Helper::concatenateFilePath( m_relationPath, m_folderPath, _filePath, _fullPath, MENGINE_MAX_PATH );

        MENGINE_UNUSED( successful );

        MENGINE_ASSERTION_FATAL( successful == true, "invalid concatenate fullPath '%s':'%s'"
            , m_folderPath.c_str()
            , _filePath.c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileGroupDirectory::existFile( const FilePath & _filePath, bool _recursive ) const
    {
        // SDL doesn't have this, so we're emulating ... ugly way :(

        Char fullPath[MENGINE_MAX_PATH];
        if( this->getFullPath( _filePath, fullPath ) == false )
        {
            LOGGER_ERROR( "invalid get fullPath '%s'"
                , _filePath.c_str()
            );

            return false;
        }

        SDL_RWops * rwops = SDL_RWFromFile( fullPath, "rb" );

        if( rwops != nullptr )
        {
            SDL_ClearError();
            SDL_RWclose( rwops );

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
    bool SDLFileGroupDirectory::removeFile( const FilePath & _filePath ) const
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
    bool SDLFileGroupDirectory::existDirectory( const FilePath & _folderPath, bool _recursive ) const
    {
        const FilePath & relationPath = this->getRelationPath();
        const FilePath & folderPath = this->getFolderPath();

        PathString accountString;
        accountString.append( relationPath );
        accountString.append( folderPath );
        accountString.append( _folderPath );
        accountString.append( '/' );

        bool result = PLATFORM_SERVICE()
            ->existDirectory( accountString.c_str() );

        if( _recursive == true && result == false && m_parentFileGroup != nullptr )
        {
            result = m_parentFileGroup->existDirectory( _folderPath, true );
        }

        return result;
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

        if( PLATFORM_SERVICE()
            ->existDirectory( accountString.c_str() ) == true )
        {
            return true;
        }

        if( PLATFORM_SERVICE()
            ->createDirectory( accountString.c_str() ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileGroupDirectory::moveFile( const FilePath & _oldFilePath, const FilePath & _newFilePath ) const
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
    bool SDLFileGroupDirectory::findFiles( const FilePath & _filePath, const Char * _mask, const LambdaFilePath & _lambda ) const
    {
        Char utf8_base[MENGINE_MAX_PATH];
        if( Helper::concatenateFilePath( m_relationPath, m_folderPath, FilePath::none(), utf8_base, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR( "invalid concatenate filePath '%s':'%s'"
                , m_folderPath.c_str()
                , _filePath.c_str()
            );

            return false;
        }

        Helper::pathCorrectForwardslashA( utf8_base );

        if( PLATFORM_SERVICE()
            ->findFiles( utf8_base, _filePath.c_str(), _mask, _lambda ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr SDLFileGroupDirectory::createInputFile( const FilePath & _filePath, bool _streaming, FileGroupInterface ** const _fileGroup, const DocumentPtr & _doc )
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

        SDLFileInputStreamPtr stream = m_factoryInputStream->createObject( _doc );
        
        MENGINE_ASSERTION_MEMORY_PANIC( stream );

        if( _fileGroup != nullptr )
        {
            *_fileGroup = this;
        }

        return stream;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileGroupDirectory::openInputFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, bool _streaming, bool _share )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, "failed _stream == nullptr" );

        FileInputStreamInterface * file = stdex::intrusive_get<FileInputStreamInterface *>( _stream );

        bool result = file->open( m_relationPath, m_folderPath, _filePath, _offset, _size, _streaming, _share );
        
        MENGINE_ASSERTION_RETURN( result == true, false, "failed open file '%s':'%s'"
            , m_folderPath.c_str()
            , _filePath.c_str()
        );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileGroupDirectory::closeInputFile( const InputStreamInterfacePtr & _stream )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, "failed _stream == nullptr" );

        FileInputStreamInterface * file = stdex::intrusive_get<FileInputStreamInterface *>( _stream );

        bool result = file->close();

        MENGINE_ASSERTION_RETURN( result == true, "failed close file '%s'"
            , m_folderPath.c_str()
        );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    OutputStreamInterfacePtr SDLFileGroupDirectory::createOutputFile( const DocumentPtr & _doc )
    {
        SDLFileOutputStreamPtr stream = m_factoryOutputStream->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( stream );

        return stream;
    }
    //////////////////////////////////////////////////////////////////////////	
    bool SDLFileGroupDirectory::openOutputFile( const FilePath & _filePath, const OutputStreamInterfacePtr & _stream )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, "failed _stream == nullptr" );

        FileOutputStreamInterface * file = stdex::intrusive_get<FileOutputStreamInterface *>( _stream );

        bool result = file->open( m_relationPath, m_folderPath, _filePath );
        
        MENGINE_ASSERTION_RETURN( result == true, "failed open file '%s':'%s'"
            , m_folderPath.c_str()
            , _filePath.c_str()
        );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileGroupDirectory::closeOutputFile( const OutputStreamInterfacePtr & _stream )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, "failed _stream == nullptr" );

        FileOutputStreamInterface * file = stdex::intrusive_get<FileOutputStreamInterface *>( _stream );

        bool result = file->close();

        MENGINE_ASSERTION_RETURN( result == true, "failed close file '%s'"
            , m_folderPath.c_str()
        );

        return result;
    }
}
