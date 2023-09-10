#pragma once

#include "Interface/FactoryInterface.h"

#include "BitmapFontInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class BitmapFontService
        : public ServiceBase<BitmapFontServiceInterface>
    {
    public:
        BitmapFontService();
        ~BitmapFontService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        BitmapFontGlyphDescriptionPtr createGlyphDescription( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentInterfacePtr & _doc ) override;

    protected:
        FactoryInterfacePtr m_factoryBitmapFontSymbols;
    };
}