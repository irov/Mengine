#pragma once

#include "FEFontEffectBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FEFontEffectFile
        : public FEFontEffectBase
        , public UnknownFEFileInterface
    {
        DECLARE_FACTORABLE( FEFontEffectFile );
        DECLARE_UNKNOWABLE();

    public:
        FEFontEffectFile();
        ~FEFontEffectFile() override;

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
    typedef IntrusivePtr<FEFontEffectFile, FontEffectInterface> FEFontEffectFilePtr;
    //////////////////////////////////////////////////////////////////////////
}