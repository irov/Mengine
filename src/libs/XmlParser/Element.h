#	pragma once

#	include <map>

namespace XMLParser
{
	class Parser;
	class Listener;
	class Manager;

	class Element
	{
	public:
		Element( Parser * _manager, const char * _name, const char ** _attrebutes );

	public:
		bool compare( const char * _name );
		void pushListener( Listener * _listener );

	protected:
		Parser * m_manager;

		const char * m_name;
		const char ** m_attrebutes;

		typedef std::map<const char *, const char *> TMapAttributes; 
		TMapAttributes m_mapAttributes;
	};
}