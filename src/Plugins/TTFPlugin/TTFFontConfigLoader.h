#pragma once

#include "Interface/TextFontConfigLoaderInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TTFFontConfigLoader
        : public TextFontConfigLoaderInterface
    {
    public:
        TTFFontConfigLoader();
        ~TTFFontConfigLoader() override;

    protected:
        bool load( const TextFontInterfacePtr & _font, const FileGroupInterfacePtr & _fileGroup, const ConfigInterfacePtr & _config ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TTFFontConfigLoader> TTFFontConfigLoaderPtr;
    //////////////////////////////////////////////////////////////////////////
}