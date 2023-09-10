#include "TTFFontGlyph.h"

#include "Interface/VocabularyServiceInterface.h"
#include "Interface/PrefetcherServiceInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionObservable.h"
#include "Kernel/Dataflow.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TTFFontGlyph::TTFFontGlyph()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TTFFontGlyph::~TTFFontGlyph()
    {
        MENGINE_ASSERTION_OBSERVABLE( this, "ttf font '%s'"
            , this->getName().c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    const TTFDataInterfacePtr & TTFFontGlyph::getTTFData() const
    {
        return m_dataTTF;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFontGlyph::initialize()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFFontGlyph::finalize()
    {
        m_glyphContent = nullptr;
        m_licenseContent = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFFontGlyph::setGlyphContent( const ContentInterfacePtr & _glyphContent )
    {
        m_glyphContent = _glyphContent;
    }
    //////////////////////////////////////////////////////////////////////////
    const ContentInterfacePtr & TTFFontGlyph::getGlyphContent() const
    {
        return m_glyphContent;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFFontGlyph::setLicenseContent( const ContentInterfacePtr & _licenseContent )
    {
        m_licenseContent = _licenseContent;
    }
    //////////////////////////////////////////////////////////////////////////
    const ContentInterfacePtr & TTFFontGlyph::getLicenseContent() const
    {
        return m_licenseContent;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFontGlyph::_compile()
    {
        DataflowInterfacePtr dataflowTTF = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "ttfFont" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( dataflowTTF );

        const FileGroupInterfacePtr & fileGroup = m_glyphContent->getFileGroup();
        const FilePath & filePath = m_glyphContent->getFilePath();

        DataflowContext context;
        context.filePath = filePath;

        TTFDataInterfacePtr data = Helper::getDataflow( fileGroup, filePath, dataflowTTF, &context, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( data );

        m_dataTTF = data;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFFontGlyph::_release()
    {
        m_dataTTF = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFontGlyph::prefetch( const PrefetcherObserverInterfacePtr & _observer )
    {
        DataflowInterfacePtr dataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "ttfFont" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( dataflow );

        const FileGroupInterfacePtr & fileGroup = m_glyphContent->getFileGroup();
        const FilePath & filePath = m_glyphContent->getFilePath();

        DataflowContext context;
        context.filePath = filePath;

        if( PREFETCHER_SERVICE()
            ->prefetchData( fileGroup, filePath, dataflow, &context, _observer ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFFontGlyph::unfetch()
    {
        const FileGroupInterfacePtr & fileGroup = m_glyphContent->getFileGroup();
        const FilePath & filePath = m_glyphContent->getFilePath();

        PREFETCHER_SERVICE()
            ->unfetch( fileGroup, filePath );
    }
    //////////////////////////////////////////////////////////////////////////
}
