#pragma once

#include "TTFDataInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TTFData
        : public TTFDataInterface
    {
        DECLARE_FACTORABLE( TTFData );

    public:
        TTFData();
        ~TTFData() override;

    public:
        bool acquire() override;
        void release() override;

    public:
        void setTTFMemory( const MemoryInterfacePtr & _memory );
        const MemoryInterfacePtr & getTTFMemory() const;

    public:
        void setFTFace( FT_Face _face );
        FT_Face getFTFace() const override;

    protected:
        FT_Face m_face;
        MemoryInterfacePtr m_memory;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TTFData, TTFDataInterface> TTFDataPtr;
    //////////////////////////////////////////////////////////////////////////
}