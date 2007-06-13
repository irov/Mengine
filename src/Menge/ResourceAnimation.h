#	pragma once

#	include "ResourceImpl.h"

#	include <vector>

namespace Menge
{
	class ResourceAnimation
		: public ResourceImpl
	{
		 RESOURCE_DECLARE( ResourceAnimation )

	public:
		ResourceAnimation( const std::string & _name );

	public:
		virtual size_t getSequenceCount() const;

		virtual int getSequenceDelay( size_t _sequence ) const;
		virtual size_t getSequenceIndex( size_t _sequence ) const;

		virtual const std::string & getImageMap() const;

	public:
		void loader( TiXmlElement *xml );

	private:
		struct Sequence
		{
			int delay;
			size_t index;
		};

		typedef std::vector<Sequence> TVectorSequence;
		TVectorSequence m_vectorSequence;

		std::string m_imageMap;
	};
}