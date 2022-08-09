#pragma once

#include "Interface/FactoryInterface.h"

#include "FEServiceInterface.h"

#include "FETextFontEffectFile.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Hashtable2.h"

namespace Mengine
{    
    class FEService
        : public ServiceBase<FEServiceInterface>
    {
    public:
        FEService();
        ~FEService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        TextFontEffectInterfacePtr createTextFontEffect( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _name, uint32_t _sample ) override;

    protected:
        typedef Hashtable2<ConstString, FilePath, FETextFontEffectFilePtr> HashtableTextFontEffects;
        HashtableTextFontEffects m_textFontEffects;

        FactoryInterfacePtr m_factoryFETextFontEffectFile;
        FactoryInterfacePtr m_factoryFETextFontEffectCustom;
    };
}