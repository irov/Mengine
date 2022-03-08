#include "FileStreamHelper.h"

#ifdef MENGINE_DEBUG
#include "Interface/FileInputStreamInterface.h"
#endif

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/FileGroupHelper.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        InputStreamInterfacePtr openInputStreamFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _streaming, bool _share, const DocumentPtr & _doc )
        {
            FileGroupInterface * realFileGroup;
            InputStreamInterfacePtr stream = _fileGroup->createInputFile( _filePath, _streaming, &realFileGroup, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( stream, "can't create input file '%s'"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
            );

            if( realFileGroup->openInputFile( _filePath, stream, 0, ~0U, _streaming, _share ) == false )
            {
                LOGGER_ERROR( "can't open input file '%s'"
                    , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                );

                return nullptr;
            }

            return stream;
        }
        //////////////////////////////////////////////////////////////////////////
        InputStreamInterfacePtr openInputStreamMutexFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const InputStreamInterfacePtr & _stream, const ThreadMutexInterfacePtr & _mutex, const DocumentPtr & _doc )
        {
            FileGroupInterface * realFileGroup;
            InputStreamInterfacePtr stream = _fileGroup->createInputMutexFile( _filePath, _stream, _mutex, &realFileGroup, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( stream, "can't create input mutex file '%s'"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
            );

            if( realFileGroup->openInputMutexFile( _filePath, stream, 0, ~0U ) == false )
            {
                LOGGER_ERROR( "can't open input mutex file '%s'"
                    , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                );

                return nullptr;
            }

            return stream;
        }
        //////////////////////////////////////////////////////////////////////////
        OutputStreamInterfacePtr openOutputStreamFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _withTemp, const DocumentPtr & _doc )
        {
            OutputStreamInterfacePtr file = _fileGroup->createOutputFile( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( file, "can't create output file '%s'"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
            );

            if( _fileGroup->openOutputFile( _filePath, file, _withTemp ) == false )
            {
                LOGGER_ERROR( "can't open output file '%s'"
                    , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                );

                return nullptr;
            }

            return file;
        }
        //////////////////////////////////////////////////////////////////////////
        const FilePath & getInputStreamDebugRelationPath( const InputStreamInterfacePtr & _stream )
        {
            MENGINE_UNUSED( _stream );

#ifdef MENGINE_DEBUG
            FileInputStreamInterface * stream = stdex::intrusive_dynamic_get<FileInputStreamInterface *>( _stream );

            if( stream == nullptr )
            {
                return FilePath::none();
            }

            const FilePath & relationPath = stream->getRelationPath();

            return relationPath;
#else
            return FilePath::none();
#endif
        }
        //////////////////////////////////////////////////////////////////////////
        const FilePath & getInputStreamDebugFolderPath( const InputStreamInterfacePtr & _stream )
        {
            MENGINE_UNUSED( _stream );

#ifdef MENGINE_DEBUG
            FileInputStreamInterface * stream = stdex::intrusive_dynamic_get<FileInputStreamInterface *>( _stream );

            if( stream == nullptr )
            {
                return FilePath::none();
            }

            const FilePath & folderPath = stream->getFolderPath();

            return folderPath;
#else
            return FilePath::none();
#endif
        }
        //////////////////////////////////////////////////////////////////////////
        const FilePath & getInputStreamDebugFilePath( const InputStreamInterfacePtr & _stream )
        {
            MENGINE_UNUSED( _stream );

#ifdef MENGINE_DEBUG
            FileInputStreamInterface * stream = stdex::intrusive_dynamic_get<FileInputStreamInterface *>( _stream );

            if( stream == nullptr )
            {
                return FilePath::none();
            }

            const FilePath & filePath = stream->getFilePath();

            return filePath;
#else
            return FilePath::none();
#endif
        }
        //////////////////////////////////////////////////////////////////////////
    }
}