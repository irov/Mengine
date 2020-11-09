#pragma once

#include "FETextFontEffectBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FETextFontEffectCustom
        : public FETextFontEffectBase
        , public UnknownFECustomInterface
    {
        DECLARE_UNKNOWABLE();

    public:
        FETextFontEffectCustom();
        ~FETextFontEffectCustom() override;

    public:
        bool _compile() override;
        void _release() override;

    protected:
        void setFECustom( const MemoryInterfacePtr & _memory ) override;
        const MemoryInterfacePtr & getFECustom() const override;

    protected:
        bool isValid() const override;

    protected:
        MemoryInterfacePtr m_memory;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FETextFontEffectCustom> FETextFontEffectCustomPtr;
    //////////////////////////////////////////////////////////////////////////
}