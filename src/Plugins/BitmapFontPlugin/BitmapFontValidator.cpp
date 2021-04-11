#include "BitmapFontValidator.h"

#include "BitmapFont.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    bool BitmapFontValidator::validate( const TextFontInterfacePtr & _textFont ) const
    {
        BitmapFontPtr bitmapFont = BitmapFontPtr::from( _textFont );

        const ContentInterfacePtr & content = bitmapFont->getContent();

        if( content->existContent() == false )
        {
            const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
            const FilePath & filePath = content->getFilePath();

            LOGGER_ERROR( "bitmap font '%s' not found file '%s:%s'"
                , bitmapFont->getName().c_str()
                , fileGroup->getName().c_str()
                , filePath.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}