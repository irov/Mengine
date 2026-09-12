#pragma once

#include "FontEffectPluginInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class FontEffectService
        : public ServiceBase<FontEffectServiceInterface>
    {
        DECLARE_FACTORABLE( FontEffectService );

    public:
        FontEffectService();
        ~FontEffectService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        FontEffectInterfacePtr createFontEffect( const FontEffectDesc & _desc, uint32_t _sample, const DocumentInterfacePtr & _doc ) override;
        FontEffectInterfacePtr createFontEffectFromFile( const ContentInterfacePtr & _content, const ConstString & _effectName, uint32_t _sample, const DocumentInterfacePtr & _doc ) override;
    };
}
