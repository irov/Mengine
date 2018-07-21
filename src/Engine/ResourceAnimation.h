#pragma once

#include "Kernel/Resource.h"

#include "Config/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class ResourceImage, class Resource> ResourceImagePtr;
	//////////////////////////////////////////////////////////////////////////
	struct AnimationSequence
	{
		ResourceImagePtr resource;
		float delay;
		ConstString resourceName;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef Vector<AnimationSequence> TVectorAnimationSequence;
	//////////////////////////////////////////////////////////////////////////
	class ResourceAnimation
		: public Resource
	{
		DECLARE_VISITABLE( Resource );

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
	typedef IntrusiveResourcePtr<ResourceAnimation> ResourceAnimationPtr;
}





