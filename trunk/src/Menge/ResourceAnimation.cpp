#	include "ResourceAnimation.h"
#	include "Kernel/ResourceImplement.h"

#	include "ResourceImage.h"

#   include "Interface/ResourceInterface.h"

#	include "Metacode.h"
#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceAnimation );
	//////////////////////////////////////////////////////////////////////////
	ResourceAnimation::ResourceAnimation()
		: m_duration(0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceAnimation::_loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceAnimation * metadata
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceAnimation *>(_meta);

        const Metacode::Meta_DataBlock::Meta_ResourceAnimation::TVectorMeta_Sequence & includes_sequence = metadata->get_IncludesSequence();

        for( Metacode::Meta_DataBlock::Meta_ResourceAnimation::TVectorMeta_Sequence::const_iterator
            it = includes_sequence.begin(),
            it_end = includes_sequence.end();
        it != it_end;
        ++it )
        {
            const Metacode::Meta_DataBlock::Meta_ResourceAnimation::Meta_Sequence & meta_sequence = *it;

            AnimationSequence sq;

            meta_sequence.swap_ResourceImageName( sq.resourceName );
            sq.delay = meta_sequence.get_Delay();

            m_sequence.push_back( sq );
        }

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceAnimation::_compile()
	{
		if( m_sequence.empty() )
		{
			LOGGER_ERROR(m_serviceProvider)( "ResourceAnimation::_compile: '%s' sequence count is empty"
				, this->getName().c_str()
				);

			return false;
		}

		m_duration = 0.f;

		for( TVectorAnimationSequence::iterator
			it = m_sequence.begin(),
			it_end = m_sequence.end();
		it != it_end;
		++it )
		{
			AnimationSequence & sequence = *(it);
			
			ResourceImage * resource  = RESOURCE_SERVICE(m_serviceProvider)
				->getResourceT<ResourceImage>( sequence.resourceName );

			if( resource == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)( "ResourceAnimation::_compile: '%s' Image resource not found resource '%s'"
					, this->getName().c_str()
					, sequence.resourceName.c_str() 
					);

				return false;
			}

			sequence.resource = resource;

			m_duration += sequence.delay;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceAnimation::_release()
	{
		for( TVectorAnimationSequence::iterator
			it = m_sequence.begin(),
			it_end = m_sequence.end();
		it != it_end;
		++it )
		{
			AnimationSequence & sequence = *(it);
            		
			sequence.resource->decrementReference();
			sequence.resource = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceAnimation::getSequenceCount() const
	{
        size_t size = m_sequence.size();

		return size;
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceAnimation::getSequenceDelay( size_t _index ) const
	{
        const AnimationSequence & sequence = m_sequence[_index];

        float delay = sequence.delay;

		return delay;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceAnimation::getSequenceResourceName( size_t _index ) const
	{
        const AnimationSequence & sequence = m_sequence[_index];

        const ConstString & resourceName = sequence.resourceName;

		return resourceName;
	}	
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceAnimation::getLastFrameIndex() const
	{
        size_t size = m_sequence.size();
        size_t lastIndex = size - 1;

		return lastIndex;	
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceAnimation::setSequences( const TVectorAnimationSequence & _sequence )
	{
		m_sequence = _sequence;
	}
	//////////////////////////////////////////////////////////////////////////
	const TVectorAnimationSequence & ResourceAnimation::getSequences() const
	{
		return m_sequence;
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceImage * ResourceAnimation::getSequenceResource( size_t _index ) const
	{
        const AnimationSequence & sequence = m_sequence[_index];

        ResourceImage * resource = sequence.resource;

		return resource;
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceAnimation::getSequenceDuration() const
	{
		return m_duration;
	}
}
