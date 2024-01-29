#include "ContentHelper.h"

#include "FileGroupHelper.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/PrototypeHelper.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        ContentInterfacePtr makeFileContent( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentInterfacePtr & _doc )
        {
            ContentInterfacePtr content = Helper::generatePrototype( STRINGIZE_STRING_LOCAL( "Content" ), STRINGIZE_STRING_LOCAL( "File" ), _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( content, "invalid create content '%s'"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
            );

            content->setFileGroup( _fileGroup );
            content->setFilePath( _filePath );

            return content;
        }
        //////////////////////////////////////////////////////////////////////////
        const Char * getContentFullPath( const ContentInterfacePtr & _content )
        {
            static MENGINE_THREAD_LOCAL Char fullPath[MENGINE_MAX_PATH] = {'\0'};

            if( _content == nullptr )
            {
                return "NULL";
            }

            const FileGroupInterfacePtr & fileGroup = _content->getFileGroup();
            const FilePath & filePath = _content->getFilePath();

            fileGroup->getFullPath( filePath, fullPath );

            return fullPath;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}