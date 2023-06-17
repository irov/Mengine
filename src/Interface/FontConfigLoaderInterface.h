#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/FontInterface.h"
#include "Interface/ConfigInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FontConfigLoaderInterface
        : public ServantInterface
    {
    public:
        virtual bool load( const FontInterfacePtr & _font, const FileGroupInterfacePtr & _fileGroup, const ConfigInterfacePtr & _config ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FontConfigLoaderInterface> FontConfigLoaderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
