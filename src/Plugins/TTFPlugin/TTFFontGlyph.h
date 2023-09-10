#pragma once

#include "Interface/FontGlyphInterface.h"
#include "Interface/ContentInterface.h"

#include "TTFIncluder.h"
#include "TTFInterface.h"
#include "TTFDataInterface.h"

#include "Kernel/CompilableReference.h"
#include "Kernel/GlyphCode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TTFFontGlyph
        : public FontGlyphInterface
        , public CompilableReference
    {
        DECLARE_FACTORABLE( TTFFontGlyph );

    public:
        TTFFontGlyph();
        ~TTFFontGlyph() override;

    public:
        const TTFDataInterfacePtr & getTTFData() const;

    public:
        bool initialize() override;
        void finalize() override;

    public:
        void setGlyphContent( const ContentInterfacePtr & _glyphContent );
        const ContentInterfacePtr & getGlyphContent() const;

    public:
        void setLicenseContent( const ContentInterfacePtr & _licenseContent );
        const ContentInterfacePtr & getLicenseContent() const;

    protected:
        bool _compile() override;
        void _release() override;

    public:
        bool prefetch( const PrefetcherObserverInterfacePtr & _observer ) override;
        void unfetch() override;

    protected:        
        TTFDataInterfacePtr m_dataTTF;

        ContentInterfacePtr m_glyphContent;
        ContentInterfacePtr m_licenseContent;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TTFFontGlyph, FontGlyphInterface> TTFFontGlyphPtr;
    //////////////////////////////////////////////////////////////////////////
}