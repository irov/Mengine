#include "FileStreamHelper.h"

#if defined(MENGINE_DEBUG)
#   include "Interface/FileInputStreamInterface.h"
#   include "Interface/FileOutputStreamInterface.h"
#endif

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/FileGroupHelper.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        InputStreamInterfacePtr openInputStreamFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _streaming, bool _share, const DocumentInterfacePtr & _doc )
        {
            FileGroupInterface * realFileGroup;
            InputStreamInterfacePtr stream = _fileGroup->createInputFile( _filePath, _streaming, &realFileGroup, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( stream, "can't create input file '%s'"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
            );

            if( realFileGroup->openInputFile( _filePath, stream, 0, MENGINE_UNKNOWN_SIZE, _streaming, _share ) == false )
            {
                LOGGER_ERROR( "can't open input file '%s'"
                    , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                );

                return nullptr;
            }

            return stream;
        }
        //////////////////////////////////////////////////////////////////////////
        InputStreamInterfacePtr openInputStreamMutexFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const InputStreamInterfacePtr & _stream, const ThreadMutexInterfacePtr & _mutex, const DocumentInterfacePtr & _doc )
        {
            FileGroupInterface * realFileGroup;
            InputStreamInterfacePtr stream = _fileGroup->createInputMutexFile( _filePath, _stream, _mutex, &realFileGroup, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( stream, "can't create input mutex file '%s'"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
            );

            if( realFileGroup->openInputMutexFile( _filePath, stream, 0, MENGINE_UNKNOWN_SIZE ) == false )
            {
                LOGGER_ERROR( "can't open input mutex file '%s'"
                    , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                );

                return nullptr;
            }

            return stream;
        }
        //////////////////////////////////////////////////////////////////////////
        OutputStreamInterfacePtr openOutputStreamFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _withTemp, const DocumentInterfacePtr & _doc )
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
        bool closeInputStreamFile( const FileGroupInterfacePtr & _fileGroup, const InputStreamInterfacePtr & _stream )
        {
            if( _fileGroup->closeInputFile( _stream ) == false )
            {
                const FilePath & filePath = Helper::getInputStreamDebugRelationPath( _stream );

                LOGGER_ERROR( "invalid close input file '%s'"
                    , Helper::getFileGroupFullPath( _fileGroup, filePath )
                );

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool closeOutputStreamFile( const FileGroupInterfacePtr & _fileGroup, const OutputStreamInterfacePtr & _stream )
        {
            if( _fileGroup->closeOutputFile( _stream ) == false )
            {
                const FilePath & filePath = Helper::getOutputStreamDebugFilePath( _stream );

                LOGGER_ERROR( "invalid close output file '%s'"
                    , Helper::getFileGroupFullPath( _fileGroup, filePath )
                );

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeOutputStreamFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _withTemp, const MemoryInterfacePtr & _memory, const DocumentInterfacePtr & _doc )
        {
            OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( _fileGroup, _filePath, _withTemp, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( stream, "can't open output file '%s'"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
            );

            const void * memory_buffer = _memory->getBuffer();
            size_t memory_size = _memory->getSize();

            if( stream->write( memory_buffer, memory_size ) == false )
            {
                LOGGER_ERROR( "invalid write output file '%s'"
                    , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                );

                return false;
            }

            if( Helper::closeOutputStreamFile( _fileGroup, stream ) == false )
            {
                LOGGER_ERROR( "invalid close output file '%s'"
                    , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                );

                return false;
            }

            stream = nullptr;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        const FilePath & getInputStreamDebugRelationPath( const InputStreamInterfacePtr & _stream )
        {
            MENGINE_UNUSED( _stream );

#if defined(MENGINE_DEBUG)
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

#if defined(MENGINE_DEBUG)
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

#if defined(MENGINE_DEBUG)
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
        const FilePath & getOutputStreamDebugRelationPath( const OutputStreamInterfacePtr & _stream )
        {
            MENGINE_UNUSED( _stream );

#if defined(MENGINE_DEBUG)
            FileOutputStreamInterface * stream = stdex::intrusive_dynamic_get<FileOutputStreamInterface *>( _stream );

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
        const FilePath & getOutputStreamDebugFolderPath( const OutputStreamInterfacePtr & _stream )
        {
            MENGINE_UNUSED( _stream );

#if defined(MENGINE_DEBUG)
            FileOutputStreamInterface * stream = stdex::intrusive_dynamic_get<FileOutputStreamInterface *>( _stream );

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
        const FilePath & getOutputStreamDebugFilePath( const OutputStreamInterfacePtr & _stream )
        {
            MENGINE_UNUSED( _stream );

#if defined(MENGINE_DEBUG)
            FileOutputStreamInterface * stream = stdex::intrusive_dynamic_get<FileOutputStreamInterface *>( _stream );

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