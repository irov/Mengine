#include "ResourceImageSequence.h"

#include "Interface/ResourceServiceInterface.h"

#include "Kernel/ResourceImage.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceImageSequence::ResourceImageSequence()
        : m_duration( 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceImageSequence::~ResourceImageSequence()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageSequence::_compile()
    {
        if( m_sequence.empty() == true )
        {
            LOGGER_ERROR( "resource image sequence '%s' sequence count is empty"
                , this->getName().c_str()
            );

            return false;
        }

        uint32_t sequenceCount = this->getSequenceCount();

        for( uint32_t index = 0; index != sequenceCount; ++index )
        {
            const FrameImageSequence & sequence = m_sequence[index];

            if( sequence.resourceImage->compile() == false )
            {
                LOGGER_ERROR( "resource image sequence '%s' not compile image resource '%s'"
                    , this->getName().c_str()
                    , sequence.resourceImage->getName().c_str()
                );

                this->releaseSequence_( index );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageSequence::_release()
    {
        uint32_t sequenceCount = this->getSequenceCount();

        this->releaseSequence_( sequenceCount );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageSequence::releaseSequence_( uint32_t _count )
    {
        for( uint32_t index = 0; index != _count; ++index )
        {
            const FrameImageSequence & sequence = m_sequence[index];

            sequence.resourceImage->release();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ResourceImageSequence::getSequenceCount() const
    {
        VectorFrameImageSequence::size_type sequenceCount = m_sequence.size();

        return (uint32_t)sequenceCount;
    }
    //////////////////////////////////////////////////////////////////////////
    float ResourceImageSequence::getSequenceDelay( uint32_t _index ) const
    {
#if defined(MENGINE_DEBUG)
        uint32_t sequenceCount = this->getSequenceCount();

        if( _index >= sequenceCount )
        {
            LOGGER_ERROR( "resource image sequence '%s' sequence '%u' out of range '%u'"
                , this->getName().c_str()
                , _index
                , sequenceCount
            );

            return 0.f;
        }
#endif

        const FrameImageSequence & sequence = m_sequence[_index];

        float delay = sequence.delay;

        return delay;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ResourceImageSequence::getLastFrameIndex() const
    {
        uint32_t sequenceCount = this->getSequenceCount();

#if defined(MENGINE_DEBUG)
        if( sequenceCount == 0 )
        {
            LOGGER_ERROR( "resource image sequence '%s' invalid get last frame on empty sequences"
                , this->getName().c_str()
            );

            return 0;
        }
#endif

        uint32_t lastIndex = sequenceCount - 1;

        return lastIndex;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageSequence::addFrame( const ResourceImagePtr & _resourceImage, float _delay )
    {
        MENGINE_ASSERTION_FATAL( this->isCompile() == false, "resource '%s' already compile"
            , this->getName().c_str()
        );

        FrameImageSequence frame;
        frame.resourceImage = _resourceImage;
        frame.delay = _delay;

        m_sequence.emplace_back( frame );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageSequence::setSequence( const VectorFrameImageSequence & _sequence )
    {
        MENGINE_ASSERTION_FATAL( m_sequence.empty() == true, "resource '%s' already setup sequence"
            , this->getName().c_str()
        );

        MENGINE_ASSERTION_FATAL( this->isCompile() == false, "resource '%s' already compile"
            , this->getName().c_str()
        );

        m_sequence = _sequence;
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorFrameImageSequence & ResourceImageSequence::getSequence() const
    {
        return m_sequence;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & ResourceImageSequence::getSequenceResource( uint32_t _index ) const
    {
        MENGINE_ASSERTION_FATAL( _index < this->getSequenceCount(), "'%s' sequence '%u' out of range '%u'"
            , this->getName().c_str()
            , _index
            , this->getSequenceCount()
        );

        const FrameImageSequence & sequence = m_sequence[_index];

        const ResourceImagePtr & resource = sequence.resourceImage;

        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageSequence::setSequenceDuration( float _duration )
    {
        m_duration = _duration;
    }
    //////////////////////////////////////////////////////////////////////////
    float ResourceImageSequence::getSequenceDuration() const
    {
        return m_duration;
    }
    //////////////////////////////////////////////////////////////////////////
}
