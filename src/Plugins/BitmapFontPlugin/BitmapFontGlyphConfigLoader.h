#pragma once

#include "Interface/ConfigLoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class BitmapFontGlyphConfigLoader
        : public ConfigLoaderInterface
        , public Factorable
    {
    public:
        BitmapFontGlyphConfigLoader();
        ~BitmapFontGlyphConfigLoader() override;

    protected:
        bool load( const FactorablePtr & _factorable, const FileGroupInterfacePtr & _fileGroup, const ConfigInterfacePtr & _config ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<BitmapFontGlyphConfigLoader, ConfigLoaderInterface> BitmapFontGlyphConfigLoaderPtr;
    //////////////////////////////////////////////////////////////////////////
}