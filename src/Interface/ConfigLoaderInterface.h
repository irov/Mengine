#pragma once

#include "Interface/FileGroupInterface.h"
#include "Interface/ConfigInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ConfigLoaderInterface
        : public Mixin
    {
    public:
        virtual bool load( const FactorablePtr & _factorable, const FileGroupInterfacePtr & _fileGroup, const ConfigInterfacePtr & _config, const DocumentInterfacePtr & _doc ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ConfigLoaderInterface> ConfigLoaderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}