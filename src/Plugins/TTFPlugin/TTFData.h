#pragma once

#include "TTFDataInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TTFData
        : public TTFDataInterface
        , public Factorable
    {
    public:
        TTFData();
        ~TTFData() override;

    public:
        bool acquire() override;
        void release() override;

    protected:
        Pointer allocateMemory( size_t _size ) const override;

    public:
        void setTTFMemory( const MemoryInterfacePtr & _memory );
        const MemoryInterfacePtr & getTTFMemory() const override;

    public:
        void setFTFace( FT_Face _face );
        FT_Face getFTFace() const override;

    protected:
        FT_Face m_face;
        MemoryInterfacePtr m_memory;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TTFData> TTFDataPtr;
    //////////////////////////////////////////////////////////////////////////
}