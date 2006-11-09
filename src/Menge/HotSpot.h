#	pragma once

#	include "Allocator.h"

namespace Menge
{
	class HotSpot
		: public Allocator
	{
		OBJECT_DECLARE(HotSpot)
	
	public:
		void addPoint( const mt::vec2f &p );
		bool testPoint( const mt::vec2f &p );

	public:
		void loader( TiXmlElement *_xml) override;
		
			
	private:
		typedef std::list<mt::vec2f> TListPoint;
		TListPoint m_listPoint;
	};
}