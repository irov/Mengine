#pragma once

#include "Interface/FactoryInterface.h"

#include "FEServiceInterface.h"

#include "FEFontEffectFile.h"

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
        FontEffectInterfacePtr createFontEffect( const ContentInterfacePtr & _content, const ConstString & _name, uint32_t _sample ) override;

    protected:
        typedef Hashtable2<ConstString, FilePath, FEFontEffectFilePtr> HashtableFontEffects;
        HashtableFontEffects m_fontEffects;

        FactoryInterfacePtr m_factoryFEFontEffectFile;
        FactoryInterfacePtr m_factoryFEFontEffectCustom;
    };
}