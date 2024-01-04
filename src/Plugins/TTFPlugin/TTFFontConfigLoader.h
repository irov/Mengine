#pragma once

#include "Interface/ConfigLoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TTFFontConfigLoader
        : public ConfigLoaderInterface
        , public Factorable
    {
    public:
        TTFFontConfigLoader();
        ~TTFFontConfigLoader() override;

    protected:
        bool load( const FactorablePtr & _factorable, const FileGroupInterfacePtr & _fileGroup, const ConfigInterfacePtr & _config, const DocumentInterfacePtr & _doc ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TTFFontConfigLoader, ConfigLoaderInterface> TTFFontConfigLoaderPtr;
    //////////////////////////////////////////////////////////////////////////
}