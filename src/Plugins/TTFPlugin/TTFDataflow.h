#pragma once

#include "Interface/DataflowInterface.h"
#include "Interface/ArchivatorInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Factory.h"

#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"

namespace Mengine
{
    class TTFDataflow
        : public DataflowInterface
        , public Factorable
    {
    public:
        TTFDataflow();
        ~TTFDataflow() override;

    public:
        void setFTLibrary( FT_Library _library );
        FT_Library getFTLibrary() const;

    public:
        bool initialize() override;
        void finalize() override;

    public:
        DataInterfacePtr create() override;

    public:
        bool load( const DataInterfacePtr & _data, const InputStreamInterfacePtr & _stream ) override;

    protected:
        FT_Library m_library;

        FactoryPtr m_factoryTTFData;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TTFDataflow, DataflowInterface> TTFDataflowPtr;
    //////////////////////////////////////////////////////////////////////////
}