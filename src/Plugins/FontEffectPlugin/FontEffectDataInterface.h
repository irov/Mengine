#pragma once

#include "Interface/DataInterface.h"

#include "Kernel/ConstString.h"

#include "Config/Lambda.h"

#include "FontEffectDesc.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FontEffectDataInterface
        : public DataInterface
    {
    public:
        virtual bool findEffect( const ConstString & _name, FontEffectDesc * const _desc, uint32_t * const _sample ) const = 0;

    public:
        typedef Lambda<void( const ConstString & _name, const FontEffectDesc & _desc, uint32_t _sample )> LambdaFontEffectEntry;
        virtual void foreachEffect( const LambdaFontEffectEntry & _lambda ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FontEffectDataInterface, DataInterface> FontEffectDataInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
