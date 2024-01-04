#pragma once

#include "Interface/Interface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/ConfigInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ConfigLoaderInterface
        : public Interface
    {
    public:
        virtual bool load( const FactorablePtr & _factorable, const FileGroupInterfacePtr & _fileGroup, const ConfigInterfacePtr & _config, const DocumentInterfacePtr & _doc ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ConfigLoaderInterface> ConfigLoaderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}