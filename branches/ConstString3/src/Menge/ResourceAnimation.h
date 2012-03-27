#	pragma once

#	include "ResourceReference.h"

#	include <vector>

namespace Menge
{
	struct AnimationSequence
	{
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

		size_t getSequenceCount() const;
		float getSequenceDelay( size_t _sequence ) const;
		const ConstString& getSequenceResourceName( size_t _sequence ) const;
		size_t getLastFrameIndex() const;

	public:
		void setSequences( const TVectorAnimationSequence & _sequence );
		const TVectorAnimationSequence & getSequences() const;
		void loader( BinParser * _parser ) override;
	protected:
		bool _compile() override;

	private:		
		TVectorAnimationSequence m_sequence;
	};
}





