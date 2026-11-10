#include "TextBidirectional.h"

#include "Interface/TextAlgorithmInterface.h"

#include "Kernel/Assertion.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/VocabularyHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TextBidirectional::TextBidirectional()
        : m_textSize( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TextBidirectional::~TextBidirectional()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextBidirectional::initialize( const U32String & _text )
    {
        MENGINE_ASSERTION_FATAL( m_bidirectional == nullptr, "text bidirectional already initialized" );

        m_textSize = _text.size();

        TextAlgorithmInterfacePtr algorithm = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "TextAlgorithm" ), STRINGIZE_STRING_LOCAL( "Unicode" ) );

        if( algorithm == nullptr )
        {
            return true;
        }

        m_bidirectional = algorithm->createBidirectional( _text, MENGINE_DOCUMENT_FUNCTION );

        if( m_bidirectional == nullptr )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextBidirectional::getVisualSegments( size_t _begin, size_t _end, VectorTextBidirectionalSegments * const _segments ) const
    {
        MENGINE_ASSERTION_FATAL( _begin <= _end, "invalid text bidirectional range [%zu, %zu)"
            , _begin
            , _end
        );

        MENGINE_ASSERTION_FATAL( _end <= m_textSize, "text bidirectional range end %zu out of text size %zu"
            , _end
            , m_textSize
        );

        if( m_bidirectional == nullptr )
        {
            if( _begin == _end )
            {
                return true;
            }

            TextBidirectionalSegmentDesc desc;
            desc.offset = _begin;
            desc.length = _end - _begin;
            desc.direction = ETD_LEFT_TO_RIGHT;
            _segments->emplace_back( desc );

            return true;
        }

        if( m_bidirectional->getVisualSegments( _begin, _end, _segments ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
