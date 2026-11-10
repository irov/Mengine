#include "SheenBidiTextAlgorithm.h"

#include "SheenBidiTextBidirectional.h"

#include "Kernel/FactorableUnique.h"

#include "SheenBidi/SBCodepoint.h"
#include "SheenBidi/SBScriptLocator.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SheenBidiTextAlgorithm::SheenBidiTextAlgorithm()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SheenBidiTextAlgorithm::~SheenBidiTextAlgorithm()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TextBidirectionalInterfacePtr SheenBidiTextAlgorithm::createBidirectional( const U32String & _text, const DocumentInterfacePtr & _doc ) const
    {
        SheenBidiTextBidirectionalPtr bidirectional = Helper::makeFactorableUnique<SheenBidiTextBidirectional>( _doc );

        if( bidirectional->initialize( _text ) == false )
        {
            return nullptr;
        }

        return bidirectional;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SheenBidiTextAlgorithm::getScriptSegments( const uint32_t * _text, size_t _size, VectorTextScriptSegments * const _segments ) const
    {
        if( _size == 0 )
        {
            return true;
        }

        SBScriptLocatorRef locator = SBScriptLocatorCreate();

        if( locator == nullptr )
        {
            return false;
        }

        SBCodepointSequence sequence = {SBStringEncodingUTF32, _text, _size};
        SBScriptLocatorLoadCodepoints( locator, &sequence );

        while( SBScriptLocatorMoveNext( locator ) == SBTrue )
        {
            const SBScriptAgent * agent = SBScriptLocatorGetAgent( locator );

            TextScriptSegmentDesc segment;
            segment.offset = agent->offset;
            segment.length = agent->length;
            segment.scriptTag = SBScriptGetUnicodeTag( agent->script );
            _segments->emplace_back( segment );
        }

        SBScriptLocatorRelease( locator );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    Char32 SheenBidiTextAlgorithm::getMirroredCodepoint( Char32 _code ) const
    {
        SBCodepoint mirror = SBCodepointGetMirror( _code );

        if( mirror == 0 )
        {
            return _code;
        }

        return (Char32)mirror;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SheenBidiTextAlgorithm::isCombiningMark( Char32 _code ) const
    {
        SBGeneralCategory category = SBCodepointGetGeneralCategory( _code );

        bool mark = SBGeneralCategoryIsMark( category );

        return mark;
    }
    //////////////////////////////////////////////////////////////////////////
}
