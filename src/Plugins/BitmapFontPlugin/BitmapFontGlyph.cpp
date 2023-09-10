#include "BitmapFontGlyph.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BitmapFontGlyph::BitmapFontGlyph()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BitmapFontGlyph::~BitmapFontGlyph()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool BitmapFontGlyph::initialize()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapFontGlyph::finalize()
    {
        m_fontGlyphDescrtiption = nullptr;

        m_imageContent = nullptr;
        m_licenseContent = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapFontGlyph::setBitmapFontGlyphDescription( const BitmapFontGlyphDescriptionPtr & _fontGlyphDescrtiption )
    {
        m_fontGlyphDescrtiption = _fontGlyphDescrtiption;
    }
    //////////////////////////////////////////////////////////////////////////
    const BitmapFontGlyphDescriptionPtr & BitmapFontGlyph::getBitmapFontGlyphDescription() const
    {
        return m_fontGlyphDescrtiption;
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapFontGlyph::setBitmapImageContent( const ContentInterfacePtr & _imageContent )
    {
        m_imageContent = _imageContent;
    }
    //////////////////////////////////////////////////////////////////////////
    const ContentInterfacePtr & BitmapFontGlyph::getBitmapImageContent() const
    {
        return m_imageContent;
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapFontGlyph::setBitmapLicenseContent( const ContentInterfacePtr & _licenseContent )
    {
        m_licenseContent = _licenseContent;
    }
    //////////////////////////////////////////////////////////////////////////
    const ContentInterfacePtr & BitmapFontGlyph::getBitmapLicenseContent() const
    {
        return m_licenseContent;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BitmapFontGlyph::prefetch( const PrefetcherObserverInterfacePtr & _observer )
    {
        MENGINE_UNUSED( _observer );

        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapFontGlyph::unfetch()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
