#pragma once

#include "FETextFontEffectBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FETextFontEffectCustom
        : public FETextFontEffectBase
        , public UnknownFECustomInterface
    {
        DECLARE_FACTORABLE( FETextFontEffectCustom );
        DECLARE_UNKNOWABLE();

    public:
        FETextFontEffectCustom();
        ~FETextFontEffectCustom() override;

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
    typedef IntrusivePtr<FETextFontEffectCustom> FETextFontEffectCustomPtr;
    //////////////////////////////////////////////////////////////////////////
}