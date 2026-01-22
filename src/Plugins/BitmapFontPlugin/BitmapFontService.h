#pragma once

#include "Interface/FactoryInterface.h"

#include "BitmapFontInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class BitmapFontService
        : public ServiceBase<BitmapFontServiceInterface>
    {
        DECLARE_FACTORABLE( BitmapFontService );

    public:
        BitmapFontService();
        ~BitmapFontService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        BitmapFontGlyphDescriptionPtr createGlyphDescription( const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc ) override;

    protected:
        FactoryInterfacePtr m_factoryBitmapFontSymbols;
    };
}