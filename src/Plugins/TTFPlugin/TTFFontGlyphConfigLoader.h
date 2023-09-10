#pragma once

#include "Interface/ConfigLoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TTFFontGlyphConfigLoader
        : public ConfigLoaderInterface
        , public Factorable
    {
    public:
        TTFFontGlyphConfigLoader();
        ~TTFFontGlyphConfigLoader() override;

    protected:
        bool load( const FactorablePtr & _factorable, const FileGroupInterfacePtr & _fileGroup, const ConfigInterfacePtr & _config ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TTFFontGlyphConfigLoader, ConfigLoaderInterface> TTFFontGlyphConfigLoaderPtr;
    //////////////////////////////////////////////////////////////////////////
}