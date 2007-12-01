#	include "Element.h"
#	include "Parser.h"

#	include <string.h>

namespace XMLParser
{
	Element::Element( Parser * _manager, const char * _name, const char ** _attrebutes )
		: m_manager(_manager)
		, m_name(_name)
		, m_attrebutes(_attrebutes)
	{		
		for( size_t i = 0; m_attrebutes[i]; i += 2 )
		{
			//printf(" %s='%s'", attr[i], attr[i + 1]);
			m_mapAttributes[ m_attrebutes[i] ] = m_attrebutes[ i +1 ];
		}
	}

	bool Element::compare( const char * _name )
	{
		return strcmp( m_name, _name ) == 0;
	}

	void Element::pushListener( Listener * _listener )
	{
		m_manager->pushListener(_listener);
	}
}