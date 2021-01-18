#pragma once

#include "FETextFontEffectBase.h"

#include "Kernel/BaseContent.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FETextFontEffectFile
        : public FETextFontEffectBase
        , public UnknownFEFileInterface
        , protected BaseContent
    {
        DECLARE_UNKNOWABLE();
        DECLARE_CONTENTABLE();

    public:
        FETextFontEffectFile();
        ~FETextFontEffectFile() override;

    public:
        bool _compile() override;
        void _release() override;

    public:
        bool _prefetch( const PrefetcherObserverInterfacePtr & _observer ) override;
        bool _unfetch() override;

    protected:
        bool isValid() const override;

    protected:
        FEDataInterfacePtr m_data;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FETextFontEffectFile> FETextFontEffectFilePtr;
    //////////////////////////////////////////////////////////////////////////
}