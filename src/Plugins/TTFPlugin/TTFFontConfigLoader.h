#pragma once

#include "Interface/FontConfigLoaderInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TTFFontConfigLoader
        : public FontConfigLoaderInterface
    {
    public:
        TTFFontConfigLoader();
        ~TTFFontConfigLoader() override;

    protected:
        bool load( const FontInterfacePtr & _font, const FileGroupInterfacePtr & _fileGroup, const ConfigInterfacePtr & _config ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TTFFontConfigLoader> TTFFontConfigLoaderPtr;
    //////////////////////////////////////////////////////////////////////////
}