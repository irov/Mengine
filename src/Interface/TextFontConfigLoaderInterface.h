#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/TextFontInterface.h"
#include "Interface/ConfigInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TextFontConfigLoaderInterface
        : public ServantInterface
    {
    public:
        virtual bool load( const TextFontInterfacePtr & _font, const FileGroupInterfacePtr & _fileGroup, const ConfigInterfacePtr & _config ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TextFontConfigLoaderInterface> TextFontConfigLoaderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
