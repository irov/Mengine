#pragma once

#include "Interface/TextFontConfigLoaderInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class BitmapFontConfigLoader
        : public TextFontConfigLoaderInterface
    {
    public:
        BitmapFontConfigLoader();
        ~BitmapFontConfigLoader() override;

    protected:
        bool load( const TextFontInterfacePtr & _font, const FileGroupInterfacePtr & _fileGroup, const ConfigInterfacePtr & _config ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<BitmapFontConfigLoader> BitmapFontConfigLoaderPtr;
    //////////////////////////////////////////////////////////////////////////
}