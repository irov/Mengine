#	pragma once

#	include "ResourceReference.h"

#	include <vector>

namespace Menge
{
	struct AnimationSequence
	{
		float delay;
		size_t index;
	};

	typedef std::vector<AnimationSequence> TVectorAnimationSequence;

	class ResourceSequence
		: public ResourceReference
	{
		 RESOURCE_DECLARE( ResourceSequence )

	public:
		ResourceSequence();

	public:
		size_t getSequenceCount() const;

		float getSequenceDelay( size_t _sequence ) const;

		size_t getSequenceIndex( size_t _sequence ) const;
		size_t getLastFrameIndex() const;

	public:
		void addSequence( const AnimationSequence & _sequence );

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
