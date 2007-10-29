#	pragma once

#	include "ResourceReference.h"

#	include <vector>

namespace Menge
{
	class ResourceAnimation
		: public ResourceReference
	{
		 RESOURCE_DECLARE( ResourceAnimation )

	public:
		ResourceAnimation( const std::string & _name );

	public:
		virtual size_t getSequenceCount() const;

		virtual size_t getSequenceDelay( size_t _sequence ) const;
		virtual size_t getSequenceIndex( size_t _sequence ) const;

	public:
		void loader( TiXmlElement * _xml );

	private:
		struct Sequence
		{
			size_t delay;
			size_t index;
		};

		typedef std::vector<Sequence> TVectorSequence;
		TVectorSequence m_vectorSequence;
	};
}