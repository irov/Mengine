#pragma once

#include "FontEffectDataInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Map.h"
#include "Kernel/VectorConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FontEffectData
        : public FontEffectDataInterface
    {
        DECLARE_FACTORABLE( FontEffectData );

    public:
        FontEffectData();
        ~FontEffectData() override;

    public:
        bool acquire() override;
        void release() override;

    public:
        void addEffect( const ConstString & _name, const FontEffectDesc & _desc, uint32_t _sample );

    public:
        bool findEffect( const ConstString & _name, FontEffectDesc * const _desc, uint32_t * const _sample ) const override;
        void foreachEffect( const LambdaFontEffectEntry & _lambda ) const override;

    protected:
        struct Entry
        {
            FontEffectDesc desc;
            uint32_t sample = 0;
        };

        typedef Map<ConstString, Entry> MapEntries;
        MapEntries m_entries;

        VectorConstString m_order;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FontEffectData, DataInterface> FontEffectDataPtr;
    //////////////////////////////////////////////////////////////////////////
}
