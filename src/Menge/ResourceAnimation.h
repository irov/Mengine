#	pragma once

#	include "Kernel/ResourceReference.h"
#	include "Kernel/ResourceImage.h"

#	include <stdex/stl_vector.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
    class ResourceImage;
	//////////////////////////////////////////////////////////////////////////
	struct AnimationSequence
	{
		AnimationSequence()
			: delay(0.f)
		{}
		
		ResourceImagePtr resource;
		float delay;
		ConstString resourceName;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::vector<AnimationSequence> TVectorAnimationSequence;
	//////////////////////////////////////////////////////////////////////////
	class ResourceAnimation
		: public ResourceReference
	{
		DECLARE_VISITABLE();

	public:
		ResourceAnimation();

	public:
		uint32_t getSequenceCount() const;
		float getSequenceDelay( uint32_t _index ) const;		
		const ConstString& getSequenceResourceName( uint32_t _index ) const;
		const ResourceImagePtr & getSequenceResource( uint32_t _index ) const;

		uint32_t getLastFrameIndex() const;
		float getSequenceDuration() const;

	public:
		void setSequences( const TVectorAnimationSequence & _sequence );
		const TVectorAnimationSequence & getSequences() const;

	protected:
		bool _loader( const Metabuf::Metadata * _parser ) override;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		bool _isValid() const override;

	protected:		
		TVectorAnimationSequence m_sequence;

		float m_duration;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ResourceAnimation> ResourceAnimationPtr;
}





