#include "FileStreamHelper.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        InputStreamInterfacePtr openInputStreamFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _streaming, const Char * _doc )
        {
            FileGroupInterface * realFileGroup;
            InputStreamInterfacePtr file = _fileGroup->createInputFile( _filePath, _streaming, &realFileGroup, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( file, nullptr, "can't create input file '%s:%s'"
                , _fileGroup->getName().c_str()
                , _filePath.c_str()
            );

            if( realFileGroup->openInputFile( _filePath, file, 0, 0, _streaming ) == false )
            {
                LOGGER_ERROR( "can't open input file '%s:%s'"
                    , _fileGroup->getName().c_str()
                    , _filePath.c_str()
                );

                return nullptr;
            }

            return file;
        }
        //////////////////////////////////////////////////////////////////////////
        OutputStreamInterfacePtr openOutputStreamFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const Char * _doc )
        {
            OutputStreamInterfacePtr file = _fileGroup->createOutputFile( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( file, nullptr, "can't create output file '%s:%s'"
                , _fileGroup->getName().c_str()
                , _filePath.c_str()
            );

            if( _fileGroup->openOutputFile( _filePath, file ) == false )
            {
                LOGGER_ERROR( "can't open output file '%s:%s'"
                    , _fileGroup->getName().c_str()
                    , _filePath.c_str()
                );

                return nullptr;
            }

            return file;
        }
    }
}