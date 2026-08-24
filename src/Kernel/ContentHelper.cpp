#include "ContentHelper.h"

#include "Interface/CodecServiceInterface.h"

#include "FileGroupHelper.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/PrototypeHelper.h"

#include "Config/Path.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        ContentInterfacePtr makeFileContent( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentInterfacePtr & _doc )
        {
            ContentInterfacePtr content = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Content" ), STRINGIZE_STRING_LOCAL( "File" ), _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( content, "invalid create content '%s'"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath ).c_str()
            );

            content->setFileGroup( _fileGroup );
            content->setFilePath( _filePath );

            return content;
        }
        //////////////////////////////////////////////////////////////////////////
        const ConstString & findContentCodecType( const ContentInterfacePtr & _content )
        {
            const ConstString & codecType = _content->getCodecType();

            if( codecType.empty() == false )
            {
                return codecType;
            }

            const FilePath & filePath = _content->getFilePath();
            const ConstString & fileCodecType = CODEC_SERVICE()
                ->findCodecType( filePath );

            return fileCodecType;
        }
        //////////////////////////////////////////////////////////////////////////
        PathString getContentFullPath( const ContentInterfacePtr & _content )
        {
            if( _content == nullptr )
            {
                return PathString( "NULL" );
            }

            const FileGroupInterfacePtr & fileGroup = _content->getFileGroup();
            const FilePath & filePath = _content->getFilePath();

            Path fullPath = {'\0'};
            fileGroup->getFullPath( filePath, fullPath );

            return PathString( fullPath );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
