#pragma once

#include "FontEffectBase.h"

#include "FontEffectDataInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FontEffectFile
        : public FontEffectBase
        , public UnknownFontEffectFileInterface
    {
        DECLARE_FACTORABLE( FontEffectFile );
        DECLARE_UNKNOWABLE();

    public:
        FontEffectFile();
        ~FontEffectFile() override;

    public:
        void setContent( const ContentInterfacePtr & _content ) override;

    public:
        void setEffectName( const ConstString & _effectName ) override;
        const ConstString & getEffectName() const override;

    public:
        bool isValid() const override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        bool _prefetch( const PrefetcherObserverInterfacePtr & _observer ) override;
        bool _unfetch() override;

    protected:
        ConstString m_effectName;

        FontEffectDataInterfacePtr m_data;
    };
    //////////////////////////////////////////////////////////////////////////
}
