#include "ResourceAnimation.h"

#include "Interface/RenderSystemInterface.h"
#include "Interface/ResourceInterface.h"
#include "Interface/ConfigInterface.h"

#include "Metacode/Metacode.h"

#include "Kernel/ResourceImage.h"

#include "Logger/Logger.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	ResourceAnimation::ResourceAnimation()
		: m_duration(0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceAnimation::_loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceAnimation * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceAnimation *>(_meta);

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceAnimation::VectorMeta_Sequence & includes_sequence = metadata->get_Includes_Sequence();

		m_duration = 0.f;

        for( Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceAnimation::VectorMeta_Sequence::const_iterator
            it = includes_sequence.begin(),
            it_end = includes_sequence.end();
        it != it_end;
        ++it )
        {
            const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceAnimation::Meta_Sequence & meta_sequence = *it;

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
	bool ResourceAnimation::_compile()
	{
		if( m_sequence.empty() )
		{
			LOGGER_ERROR("ResourceAnimation::_compile: '%s' sequence count is empty"
				, this->getName().c_str()
				);

			return false;
		}
		
        for( AnimationSequence & sequence : m_sequence )
		{			
			ResourceImagePtr resource  = RESOURCE_SERVICE()
				->getResource( sequence.resourceName );

			if( resource == nullptr )
			{
				LOGGER_ERROR("ResourceAnimation::_compile: '%s' Image resource not found resource '%s'"
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
	void ResourceAnimation::_release()
	{
		for( AnimationSequence & sequence : m_sequence )
		{            		
			sequence.resource->decrementReference();
			sequence.resource = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceAnimation::_isValid() const
	{
        size_t total_memory = 0;

        for( const AnimationSequence & sequence : m_sequence )
		{
			ResourceImagePtr resourceImage;
			if( RESOURCE_SERVICE()
				->hasResourceT<ResourceImagePtr>( sequence.resourceName, &resourceImage ) == false )
			{
				LOGGER_ERROR("ResourceAnimation::_isValid: '%s' not found image resource '%s'"
					, this->getName().c_str()
					, sequence.resourceName.c_str() 
					);

				return false;
			}

            if( resourceImage->isValid() == false )
            {
                LOGGER_ERROR("ResourceAnimation::_isValid %s invalid validate sequence resource '%s'"
                    , this->getName().c_str()
                    , resourceImage->getName().c_str()
                    );

                return false;
            }

            const RenderTextureInterfacePtr & texture = resourceImage->getTexture();

            if( texture == nullptr )
            {
                continue;
            }

            const RenderImageInterfacePtr & image = texture->getImage();

            uint32_t width = texture->getWidth();
            uint32_t height = texture->getHeight();

            uint32_t HWChannels = image->getHWChannels();
            uint32_t HWDepth = image->getHWDepth();
            PixelFormat HWPixelFormat = image->getHWPixelFormat();

            size_t textureSize = Helper::getTextureMemorySize( width, height, HWChannels, HWDepth, HWPixelFormat );

            total_memory += textureSize;
		}

		uint32_t animationMemoryLimit = CONFIG_VALUE("Limit", "AnimationMemoryLimit", 4194304U ); //4kb
		
		if( total_memory > animationMemoryLimit && animationMemoryLimit != 0U )
		{
			LOGGER_ERROR("ResourceAnimation::_isValid: '%s' overflow %.2fmb max video memory %.2fmb (coeff %f)"
				, this->getName().c_str()
				, float(total_memory) / (1024.f * 1024.f)
				, float(animationMemoryLimit) / (1024.f * 1024.f)
				, float(total_memory) / float(animationMemoryLimit)
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t ResourceAnimation::getSequenceCount() const
	{
        TVectorAnimationSequence::size_type sequenceCount = m_sequence.size();

		return (uint32_t)sequenceCount;
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceAnimation::getSequenceDelay( uint32_t _index ) const
	{
#ifndef NDEBUG
        uint32_t sequenceCount = this->getSequenceCount();

        if( _index >= sequenceCount )
        {
            LOGGER_ERROR("ResourceAnimation::getSequenceDelay: '%s' sequence '%u' out of range '%u'"
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
	const ConstString & ResourceAnimation::getSequenceResourceName( uint32_t _index ) const
	{
#ifndef NDEBUG
        uint32_t sequenceCount = this->getSequenceCount();

        if( _index >= sequenceCount )
        {
            LOGGER_ERROR("ResourceAnimation::getSequenceResourceName: '%s' sequence '%u' out of range '%u'"
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
	uint32_t ResourceAnimation::getLastFrameIndex() const
	{
        uint32_t sequenceCount = this->getSequenceCount();

#ifndef NDEBUG
        if( sequenceCount == 0 )
        {
            LOGGER_ERROR("ResourceAnimation::getLastFrameIndex: '%s' invalid get last frame on empty sequences"
                , this->getName().c_str()
                );

            return 0;
        }
#endif

		uint32_t lastIndex = sequenceCount - 1;

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
	const ResourceImagePtr & ResourceAnimation::getSequenceResource( uint32_t _index ) const
	{
#ifndef NDEBUG
        uint32_t sequenceCount = this->getSequenceCount();

        if( _index >= sequenceCount )
        {
            LOGGER_ERROR("ResourceAnimation::getSequenceResource: '%s' sequence '%u' out of range '%u'"
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
	float ResourceAnimation::getSequenceDuration() const
	{
		return m_duration;
	}
}
