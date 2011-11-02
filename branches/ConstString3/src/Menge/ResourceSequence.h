#	pragma once

#	include "ResourceReference.h"

#	include <vector>

namespace Menge
{
	struct AnimationSequence
	{
		float delay;
		std::size_t index;
	};

	typedef std::vector<AnimationSequence> TVectorAnimationSequence;

	class ResourceSequence
		: public ResourceReference
	{
		 RESOURCE_DECLARE( ResourceSequence )

	public:
		ResourceSequence();

	public:
		std::size_t getSequenceCount() const;

		float getSequenceDelay( std::size_t _sequence ) const;

		std::size_t getSequenceIndex( std::size_t _sequence ) const;
		std::size_t getLastFrameIndex(  ) const;
	public:
		void setSequences( const TVectorAnimationSequence & _sequence );
		const TVectorAnimationSequence & getSequences() const;

	public:
		void loader( BinParser * _parser ) override;
		void loaderSequences_( BinParser * _parser );

	protected:
		bool _compile() override;

	private:		
		TVectorAnimationSequence m_sequence;
	};
}
