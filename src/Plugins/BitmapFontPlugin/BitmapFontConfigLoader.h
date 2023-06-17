#pragma once

#include "Interface/FontConfigLoaderInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class BitmapFontConfigLoader
        : public FontConfigLoaderInterface
    {
    public:
        BitmapFontConfigLoader();
        ~BitmapFontConfigLoader() override;

    protected:
        bool load( const FontInterfacePtr & _font, const FileGroupInterfacePtr & _fileGroup, const ConfigInterfacePtr & _config ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<BitmapFontConfigLoader> BitmapFontConfigLoaderPtr;
    //////////////////////////////////////////////////////////////////////////
}