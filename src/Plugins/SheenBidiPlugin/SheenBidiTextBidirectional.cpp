#include "SheenBidiTextBidirectional.h"

#include "Kernel/Assertion.h"

#include "Config/StdAlgorithm.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SheenBidiTextBidirectional::SheenBidiTextBidirectional()
        : m_algorithm( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SheenBidiTextBidirectional::~SheenBidiTextBidirectional()
    {
        for( SBParagraphRef paragraph : m_paragraphs )
        {
            SBParagraphRelease( paragraph );
        }

        if( m_algorithm != nullptr )
        {
            SBAlgorithmRelease( m_algorithm );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SheenBidiTextBidirectional::initialize( const U32String & _text )
    {
        MENGINE_ASSERTION_FATAL( m_algorithm == nullptr, "sheenbidi text bidirectional already initialized" );

        U32String::const_iterator begin = _text.begin();
        U32String::const_iterator end = _text.end();
        m_codepoints.assign( begin, end );

        if( m_codepoints.empty() == true )
        {
            return true;
        }

        size_t size = m_codepoints.size();
        const uint32_t * data = m_codepoints.data();
        SBCodepointSequence sequence = {SBStringEncodingUTF32, data, size};
        m_algorithm = SBAlgorithmCreate( &sequence );

        if( m_algorithm == nullptr )
        {
            return false;
        }

        size_t offset = 0;

        while( offset != size )
        {
            size_t remaining = size - offset;
            SBParagraphRef paragraph = SBAlgorithmCreateParagraph( m_algorithm, offset, remaining, SBLevelDefaultLTR );

            if( paragraph == nullptr )
            {
                return false;
            }

            m_paragraphs.emplace_back( paragraph );

            size_t paragraphLength = SBParagraphGetLength( paragraph );
            offset += paragraphLength;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SheenBidiTextBidirectional::getVisualSegments( size_t _begin, size_t _end, VectorTextBidirectionalSegments * const _segments ) const
    {
        MENGINE_ASSERTION_FATAL( _begin <= _end, "invalid sheenbidi range [%zu, %zu)"
            , _begin
            , _end
        );

        MENGINE_ASSERTION_FATAL( _end <= m_codepoints.size(), "sheenbidi range end %zu out of text size %zu"
            , _end
            , m_codepoints.size()
        );

        for( SBParagraphRef paragraph : m_paragraphs )
        {
            size_t paragraphBegin = SBParagraphGetOffset( paragraph );
            size_t paragraphLength = SBParagraphGetLength( paragraph );
            size_t paragraphEnd = paragraphBegin + paragraphLength;
            size_t begin = StdAlgorithm::max( _begin, paragraphBegin );
            size_t end = StdAlgorithm::min( _end, paragraphEnd );

            if( begin >= end )
            {
                continue;
            }

            size_t length = end - begin;
            SBLineRef line = SBParagraphCreateLine( paragraph, begin, length );

            if( line == nullptr )
            {
                return false;
            }

            SBUInteger count = SBLineGetRunCount( line );
            const SBRun * segments = SBLineGetRunsPtr( line );

            for( SBUInteger index = 0; index != count; ++index )
            {
                const SBRun & segment = segments[index];
                TextBidirectionalSegmentDesc desc;
                desc.offset = segment.offset;
                desc.length = segment.length;
                desc.direction = (segment.level & 1) == 0 ? ETD_LEFT_TO_RIGHT : ETD_RIGHT_TO_LEFT;
                _segments->emplace_back( desc );
            }

            SBLineRelease( line );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
