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
            InputStreamInterfacePtr file = _fileGroup->createInputFile( _filePath, _streaming, &realFileGroup, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( file, "can't create input file '%s'"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
            );

            if( realFileGroup->openInputFile( _filePath, file, 0, ~0U, _streaming, _share ) == false )
            {
                LOGGER_ERROR( "can't open input file '%s'"
                    , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                );

                return nullptr;
            }

            return file;
        }
        //////////////////////////////////////////////////////////////////////////
        OutputStreamInterfacePtr openOutputStreamFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentPtr & _doc )
        {
            OutputStreamInterfacePtr file = _fileGroup->createOutputFile( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( file, "can't create output file '%s'"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
            );

            if( _fileGroup->openOutputFile( _filePath, file ) == false )
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
            FileInputStreamInterface * file_stream = stdex::intrusive_dynamic_get<FileInputStreamInterface *>( _stream );

            if( file_stream == nullptr )
            {
                return FilePath::none();
            }

            const FilePath & relationPath = file_stream->getRelationPath();

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
            FileInputStreamInterface * file_stream = stdex::intrusive_dynamic_get<FileInputStreamInterface *>( _stream );

            if( file_stream == nullptr )
            {
                return FilePath::none();
            }

            const FilePath & folderPath = file_stream->getFolderPath();

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
            FileInputStreamInterface * file_stream = stdex::intrusive_dynamic_get<FileInputStreamInterface *>( _stream );

            if( file_stream == nullptr )
            {
                return FilePath::none();
            }

            const FilePath & filePath = file_stream->getFilePath();

            return filePath;
#else
            return FilePath::none();
#endif
        }
        //////////////////////////////////////////////////////////////////////////
    }
}