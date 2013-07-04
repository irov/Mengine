#	pragma once

#	include "Kernel/ResourceReference.h"

#	include <vector>

namespace Menge
{
    class ResourceImage;
	
	struct AnimationSequence
	{
		AnimationSequence()
			: resource(NULL)
			, delay(0.0f)
		{}
		
		ResourceImage * resource;
		float delay;
		ConstString resourceName;
	};

	typedef std::vector<AnimationSequence> TVectorAnimationSequence;

	class ResourceAnimation
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceAnimation )

	public:
		ResourceAnimation();

	public:
		size_t getSequenceCount() const;
		float getSequenceDelay( size_t _index ) const;		
		const ConstString& getSequenceResourceName( size_t _index ) const;
		ResourceImage *  getSequenceResource( size_t _index ) const;

		size_t getLastFrameIndex() const;
		float getSequenceDuration() const;

	public:
		void setSequences( const TVectorAnimationSequence & _sequence );
		const TVectorAnimationSequence & getSequences() const;

	protected:
		bool _loader( const Metabuf::Metadata * _parser ) override;

	protected:
		bool _compile() override;
		void _release() override;

	private:		
		TVectorAnimationSequence m_sequence;

		float m_duration;
	};
}





