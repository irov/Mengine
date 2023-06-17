#pragma once

#include "FEFontEffectBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FEFontEffectCustom
        : public FEFontEffectBase
        , public UnknownFECustomInterface
    {
        DECLARE_FACTORABLE( FEFontEffectCustom );
        DECLARE_UNKNOWABLE();

    public:
        FEFontEffectCustom();
        ~FEFontEffectCustom() override;

    protected:
        void setFECustom( const MemoryInterfacePtr & _memory ) override;
        const MemoryInterfacePtr & getFECustom() const override;

    protected:
        bool isValid() const override;

    public:
        bool _compile() override;
        void _release() override;

    protected:
        fe_bundle * m_bundle;

        MemoryInterfacePtr m_memory;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FEFontEffectCustom, FontEffectInterface> FEFontEffectCustomPtr;
    //////////////////////////////////////////////////////////////////////////
}