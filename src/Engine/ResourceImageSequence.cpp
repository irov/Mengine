#include "ResourceImageSequence.h"

#include "Interface/RenderSystemInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"

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
            LOGGER_ERROR( "'%s' sequence count is empty"
                , this->getName().c_str()
            );

            return false;
        }

        for( const FrameImageSequence & sequence : m_sequence )
        {
            if( sequence.resource->compile() == false )
            {
                LOGGER_ERROR( "resource sequence '%s' not compile image resource '%s'"
                    , this->getName().c_str()
                    , sequence.resource->getName().c_str()
                );
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageSequence::_release()
    {
        for( const FrameImageSequence & sequence : m_sequence )
        {
            sequence.resource->release();
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
#ifdef MENGINE_DEBUG
        uint32_t sequenceCount = this->getSequenceCount();

        if( _index >= sequenceCount )
        {
            LOGGER_ERROR( "'%s' sequence '%u' out of range '%u'"
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

#ifdef MENGINE_DEBUG
        if( sequenceCount == 0 )
        {
            LOGGER_ERROR( "'%s' invalid get last frame on empty sequences"
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
        FrameImageSequence frame;        
        frame.resource = _resourceImage;
        frame.delay = _delay;

        m_sequence.emplace_back( frame );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageSequence::setSequence( const VectorFrameImageSequence & _sequence )
    {
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
#ifdef MENGINE_DEBUG
        uint32_t sequenceCount = this->getSequenceCount();

        if( _index >= sequenceCount )
        {
            LOGGER_ERROR( "'%s' sequence '%u' out of range '%u'"
                , this->getName().c_str()
                , _index
                , sequenceCount
            );

            return ResourceImagePtr::none();
        }
#endif

        const FrameImageSequence & sequence = m_sequence[_index];

        const ResourceImagePtr & resource = sequence.resource;

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
}
