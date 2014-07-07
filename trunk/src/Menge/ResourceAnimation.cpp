#	include "ResourceAnimation.h"
#	include "Kernel/ResourceImplement.h"

#	include "ResourceImage.h"
#	include "ResourceImageDefault.h"

#   include "Interface/ResourceInterface.h"
#   include "Interface/ConfigInterface.h"

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
			LOGGER_ERROR(m_serviceProvider)("ResourceAnimation::_compile: '%s' sequence count is empty"
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
				LOGGER_ERROR(m_serviceProvider)("ResourceAnimation::_compile: '%s' Image resource not found resource '%s'"
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
	bool ResourceAnimation::_isValid() const
	{
		size_t total_memory = 0;

		for( TVectorAnimationSequence::const_iterator
			it = m_sequence.begin(),
			it_end = m_sequence.end();
		it != it_end;
		++it )
		{
			const AnimationSequence & sequence = *(it);

			ResourceReference * resource;
			if( RESOURCE_SERVICE(m_serviceProvider)
				->hasResource( sequence.resourceName, &resource ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("ResourceAnimation::_isValid: '%s' not found image resource '%s'"
					, this->getName().c_str()
					, sequence.resourceName.c_str() 
					);

				return false;
			}

			ResourceImage * resourceImage = dynamic_cast<ResourceImage *>(resource);

			if( resourceImage == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("ResourceAnimation::_isValid: '%s' resource '%s' not ResourceImage '%s'"
					, this->getName().c_str()
					, sequence.resourceName.c_str() 
					, resource->getType().c_str()
					);

				return false;
			}

			ResourceImageDefault * resourceImageDefault = dynamic_cast<ResourceImageDefault *>(resourceImage);

			if( resourceImageDefault != nullptr )
			{
				const ConstString & fileName = resourceImageDefault->getFileName();

				const ConstString & category = resourceImageDefault->getCategory();

				bool exist = FILE_SERVICE(m_serviceProvider)
					->existFile( category, fileName, nullptr );

				if( exist == false )
				{
					LOGGER_ERROR(m_serviceProvider)("ResourceAnimation::_isValid %s not exist file %s:%s"
						, m_name.c_str()
						, category.c_str()
						, fileName.c_str()
						);

					return false;
				}

				InputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
					->openInputFile( category, fileName );

				if( stream == nullptr )
				{
					LOGGER_ERROR(m_serviceProvider)("ResourceAnimation::_isValid %s invalid open file %s:%s"
						, m_name.c_str()
						, category.c_str()
						, fileName.c_str()
						);

					return false;
				}

				const ConstString & codecType = resourceImageDefault->getCodecType();

				ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE(m_serviceProvider)
					->createDecoderT<ImageDecoderInterfacePtr>( codecType );

				if( imageDecoder == nullptr )
				{
					LOGGER_ERROR(m_serviceProvider)("ResourceAnimation::_isValid %s file %s:%s invalid decoder %s"
						, m_name.c_str()
						, category.c_str()
						, fileName.c_str()
						, codecType.c_str()
						);

					return false;
				}

				if( imageDecoder->prepareData( stream ) == false )
				{
					LOGGER_ERROR(m_serviceProvider)("ResourceAnimation::_isValid %s file %s:%s decoder initialize failed %s"
						, m_name.c_str()
						, category.c_str()
						, fileName.c_str()
						, codecType.c_str()
						);

					return false;
				}

				const ImageCodecDataInfo * dataInfo = imageDecoder->getCodecDataInfo();
				
				size_t texture_memory = RENDERTEXTURE_SERVICE(m_serviceProvider)
					->getImageMemoryUse( dataInfo->width, dataInfo->height, dataInfo->channels, dataInfo->format );

				total_memory += texture_memory;						
			}
		}

		uint32_t animationMemoryLimit = CONFIG_VALUE(m_serviceProvider, "Limit", "AnimationMemoryLimit", 4194304U ); //4kb
		
		if( total_memory > animationMemoryLimit )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceAnimation::_isValid: '%s' overflow %.2fmb max video memory %.2fmb (coeff %f)"
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
