#pragma once

#include "Interface/FontGlyphInterface.h"
#include "Interface/FileGroupInterface.h"

#include "BitmapFontGlyphDescription.h"

#include "Kernel/Factorable.h"
#include "Kernel/GlyphCode.h"
#include "Kernel/Vector.h"

#include "math/vec4.h"
#include "math/vec2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class BitmapFontGlyph
        : public FontGlyphInterface
    {
        DECLARE_FACTORABLE( BitmapFontGlyph );

    public:
        BitmapFontGlyph();
        ~BitmapFontGlyph() override;

    public:
        bool initialize() override;
        void finalize() override;

    public:
        void setBitmapFontGlyphDescription( const BitmapFontGlyphDescriptionPtr & _fontGlyphDescrtiption );
        const BitmapFontGlyphDescriptionPtr & getBitmapFontGlyphDescription() const;

        void setBitmapImageContent( const ContentInterfacePtr & _imageContent );
        const ContentInterfacePtr & getBitmapImageContent() const;

        void setBitmapLicenseContent( const ContentInterfacePtr & _licenseContent );
        const ContentInterfacePtr & getBitmapLicenseContent() const;

    public:
        bool prefetch( const PrefetcherObserverInterfacePtr & _observer ) override;
        void unfetch() override;

    protected:
        BitmapFontGlyphDescriptionPtr m_fontGlyphDescrtiption;

        ContentInterfacePtr m_imageContent;
        ContentInterfacePtr m_licenseContent;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<BitmapFontGlyph, FontGlyphInterface> BitmapFontGlyphPtr;
    //////////////////////////////////////////////////////////////////////////
}