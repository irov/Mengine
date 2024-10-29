#include "FileStreamHelper.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/DebugFileHelper.h"

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
                , Helper::getFileGroupFullPath( _fileGroup, _filePath ).c_str()
            );

            if( realFileGroup->openInputFile( _filePath, stream, 0, MENGINE_UNKNOWN_SIZE, _streaming, _share ) == false )
            {
                LOGGER_ERROR( "can't open input file '%s'"
                    , Helper::getFileGroupFullPath( _fileGroup, _filePath ).c_str()
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
                , Helper::getFileGroupFullPath( _fileGroup, _filePath ).c_str()
            );

            if( realFileGroup->openInputMutexFile( _filePath, stream, 0, MENGINE_UNKNOWN_SIZE ) == false )
            {
                LOGGER_ERROR( "can't open input mutex file '%s'"
                    , Helper::getFileGroupFullPath( _fileGroup, _filePath ).c_str()
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
                , Helper::getFileGroupFullPath( _fileGroup, _filePath ).c_str()
            );

            if( _fileGroup->openOutputFile( _filePath, file, _withTemp ) == false )
            {
                LOGGER_ERROR( "can't open output file '%s'"
                    , Helper::getFileGroupFullPath( _fileGroup, _filePath ).c_str()
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
                const FilePath & filePath = Helper::getDebugRelationPath( _stream );

                LOGGER_ERROR( "invalid close input file '%s'"
                    , Helper::getFileGroupFullPath( _fileGroup, filePath ).c_str()
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
                const FilePath & filePath = Helper::getDebugFilePath( _stream );

                LOGGER_ERROR( "invalid close output file '%s'"
                    , Helper::getFileGroupFullPath( _fileGroup, filePath ).c_str()
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
                , Helper::getFileGroupFullPath( _fileGroup, _filePath ).c_str()
            );

            const void * memory_buffer = _memory->getBuffer();
            size_t memory_size = _memory->getSize();

            if( stream->write( memory_buffer, memory_size ) == false )
            {
                LOGGER_ERROR( "invalid write output file '%s'"
                    , Helper::getFileGroupFullPath( _fileGroup, _filePath ).c_str()
                );

                return false;
            }

            if( Helper::closeOutputStreamFile( _fileGroup, stream ) == false )
            {
                LOGGER_ERROR( "invalid close output file '%s'"
                    , Helper::getFileGroupFullPath( _fileGroup, _filePath ).c_str()
                );

                return false;
            }

            stream = nullptr;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}