#include "BitmapFontValidator.h"

#include "BitmapFont.h"

#include "Kernel/Logger.h"
#include "Kernel/FileGroupHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    bool BitmapFontValidator::validate( const FontInterfacePtr & _font ) const
    {
        BitmapFontPtr bitmapFont = BitmapFontPtr::from( _font );

        const ContentInterfacePtr & content = bitmapFont->getContent();

        if( content->existContent() == false )
        {
            const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
            const FilePath & filePath = content->getFilePath();

            LOGGER_ERROR( "bitmap font '%s' not found file '%s'"
                , bitmapFont->getName().c_str()
                , Helper::getFileGroupFullPath( fileGroup, filePath )
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}