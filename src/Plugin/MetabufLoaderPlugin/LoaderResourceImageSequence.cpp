#include "ResourceImageSequence.h"

#include "Interface/RenderSystemInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "Metacode/Metacode.h"

#include "Kernel/ResourceImage.h"

#include "Kernel/Logger.h"

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
    bool ResourceImageSequence::_loader( const Metabuf::Metadata * _meta )
    {
        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageSequence * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageSequence *>(_meta);

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageSequence::VectorMeta_Sequence & includes_sequence = metadata->get_Includes_Sequence();

        m_duration = 0.f;

        for( Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageSequence::VectorMeta_Sequence::const_iterator
            it = includes_sequence.begin(),
            it_end = includes_sequence.end();
            it != it_end;
            ++it )
        {
            const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageSequence::Meta_Sequence & meta_sequence = *it;

            AnimationSequence sq;

            sq.resourceName = meta_sequence.get_ResourceImageName();
            float delay = meta_sequence.get_Delay();

            sq.delay = delay;

            m_duration += delay;

            m_sequence.emplace_back( sq );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageSequence::_compile()
    {
        if( m_sequence.empty() )
        {
            LOGGER_ERROR( "ResourceImageSequence::_compile: '%s' sequence count is empty"
                , this->getName().c_str()
            );

            return false;
        }

        for( AnimationSequence & sequence : m_sequence )
        {
            ResourceImagePtr resource = RESOURCE_SERVICE()
                ->getResource( sequence.resourceName );

            if( resource == nullptr )
            {
                LOGGER_ERROR( "ResourceImageSequence::_compile: '%s' Image resource not found resource '%s'"
                    , this->getName().c_str()
                    , sequence.resourceName.c_str()
                );

                return false;
            }

            sequence.resource = resource;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageSequence::_release()
    {
        for( AnimationSequence & sequence : m_sequence )
        {
            sequence.resource->decrementReference();
            sequence.resource = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ResourceImageSequence::getSequenceCount() const
    {
        VectorAnimationSequence::size_type sequenceCount = m_sequence.size();

        return (uint32_t)sequenceCount;
    }
    //////////////////////////////////////////////////////////////////////////
    float ResourceImageSequence::getSequenceDelay( uint32_t _index ) const
    {
#ifndef NDEBUG
        uint32_t sequenceCount = this->getSequenceCount();

        if( _index >= sequenceCount )
        {
            LOGGER_ERROR( "ResourceImageSequence::getSequenceDelay: '%s' sequence '%u' out of range '%u'"
                , this->getName().c_str()
                , _index
                , sequenceCount
            );

            return 0.f;
        }
#endif

        const AnimationSequence & sequence = m_sequence[_index];

        float delay = sequence.delay;

        return delay;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ResourceImageSequence::getSequenceResourceName( uint32_t _index ) const
    {
#ifndef NDEBUG
        uint32_t sequenceCount = this->getSequenceCount();

        if( _index >= sequenceCount )
        {
            LOGGER_ERROR( "ResourceImageSequence::getSequenceResourceName: '%s' sequence '%u' out of range '%u'"
                , this->getName().c_str()
                , _index
                , sequenceCount
            );

            return ConstString::none();
        }
#endif

        const AnimationSequence & sequence = m_sequence[_index];

        const ConstString & resourceName = sequence.resourceName;

        return resourceName;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ResourceImageSequence::getLastFrameIndex() const
    {
        uint32_t sequenceCount = this->getSequenceCount();

#ifndef NDEBUG
        if( sequenceCount == 0 )
        {
            LOGGER_ERROR( "ResourceImageSequence::getLastFrameIndex: '%s' invalid get last frame on empty sequences"
                , this->getName().c_str()
            );

            return 0;
        }
#endif

        uint32_t lastIndex = sequenceCount - 1;

        return lastIndex;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageSequence::setSequences( const VectorAnimationSequence & _sequence )
    {
        m_sequence = _sequence;
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorAnimationSequence & ResourceImageSequence::getSequences() const
    {
        return m_sequence;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & ResourceImageSequence::getSequenceResource( uint32_t _index ) const
    {
#ifndef NDEBUG
        uint32_t sequenceCount = this->getSequenceCount();

        if( _index >= sequenceCount )
        {
            LOGGER_ERROR( "ResourceImageSequence::getSequenceResource: '%s' sequence '%u' out of range '%u'"
                , this->getName().c_str()
                , _index
                , sequenceCount
            );

            return ResourceImagePtr::none();
        }
#endif

        const AnimationSequence & sequence = m_sequence[_index];

        const ResourceImagePtr & resource = sequence.resource;

        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    float ResourceImageSequence::getSequenceDuration() const
    {
        return m_duration;
    }
}
