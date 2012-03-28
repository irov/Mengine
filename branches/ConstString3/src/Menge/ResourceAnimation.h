#	pragma once

#	include "ResourceReference.h"
#	include "ResourceImage.h"

#	include <vector>

namespace Menge
{
	
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
		float getSequenceDelay( size_t _sequence ) const;
		const ConstString& getSequenceResourceName( size_t _sequence ) const;
		ResourceImage *  getSequenceResource( size_t _sequence ) const;
		size_t getLastFrameIndex() const;

	public:
		void setSequences( const TVectorAnimationSequence & _sequence );
		const TVectorAnimationSequence & getSequences() const;

	protected:
		void loader( BinParser * _parser ) override;

	protected:
		bool _compile() override;
		void _release() override;
	private:		
		TVectorAnimationSequence m_sequence;
	};
}





