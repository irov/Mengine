#pragma once

#include "Interface/DataflowInterface.h"
#include "Interface/ThreadMutexInterface.h"
#include "Interface/FactoryInterface.h"

#include "Kernel/Factorable.h"

#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TTFDataflow
        : public DataflowInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( TTFDataflow );

    public:
        TTFDataflow();
        ~TTFDataflow() override;

    public:
        void setFTLibrary( FT_Library _library );
        FT_Library getFTLibrary() const;

    public:
        void setMutex( const ThreadMutexInterfacePtr & _mutex );
        const ThreadMutexInterfacePtr & getMutex() const;

    public:
        bool initialize() override;
        void finalize() override;

    public:
        bool isThreadFlow() const override;

    public:
        DataInterfacePtr create( const DocumentPtr & _doc ) override;

    public:
        MemoryInterfacePtr load( const InputStreamInterfacePtr & _stream, const DocumentPtr & _doc ) override;
        bool flow( const DataInterfacePtr & _data, const MemoryInterfacePtr & _memory, const DataflowContext * _context, const DocumentPtr & _doc ) override;

    protected:
        FT_Library m_library;

        ThreadMutexInterfacePtr m_mutex;

        FactoryInterfacePtr m_factoryTTFData;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TTFDataflow, DataflowInterface> TTFDataflowPtr;
    //////////////////////////////////////////////////////////////////////////
}